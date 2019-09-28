#include <Fl/fl_message.H>
#include <iostream>
#include "MainUI.h"
#include "MemoryUI.h"
#include "WatchesUI.h"
#include "ConfigClass.h"
#include "CommsClass.h"
#include "debugger.h"
#include "monitor_defs.h"
#include "mips_disassembler.h"

// Externs
extern MainUI		*ui;
extern MemoryUI		*mem_ui;
extern WatchesUI	*watch_ui;
extern unsigned int reg_buffer[];
extern int run_to_cursor;
extern int db_wait_response;
extern int db_wait_timeout;

extern ProjectClass project;
extern ConfigClass config;
extern CommsClass comms;

int RefreshRemoteCode(int force = false);
void RefreshRemoteData(int force = false);
void UpdateViews();
void HandleCommsCode(int code);

// Toggles step controls
void StepControls(int enable) {
	
	if( enable ) {
		
		ui->stepButton->activate();
		ui->stepOverButton->activate();
		ui->stepOutButton->activate();
		
	} else {
		
		ui->stepButton->deactivate();
		ui->stepOverButton->deactivate();
		ui->stepOutButton->deactivate();
		
	}
	
}

// Activate and deactivate debug controls
void DebugControls(int enable) {

	if( enable ) {
	
		ui->execControls->activate();
		StepControls(false);

		ui->regRefresh->activate();
		ui->rebootButton->activate();

		ui->connectButton->deactivate();
		ui->disconnectButton->activate();

		ui->remoteMode->activate();

		ui->statusBar->SetConnected(1);
		ui->updateToggle->activate();
		
		watch_ui->watchTable->values_active(true);
		watch_ui->updateButton->activate();
		
	} else {

		ui->execControls->deactivate();
		StepControls(false);

		ui->codeRefresh->deactivate();
		ui->regRefresh->deactivate();
		ui->rebootButton->deactivate();

		ui->connectButton->activate();
		ui->disconnectButton->deactivate();

		ui->remoteMode->deactivate();
		ui->localMode->value(1);
		ui->remoteMode->value(0);
		ui->updateToggle->deactivate();
			
		ui->statusBar->SetConnected( 0 );
		mem_ui->memory->redraw();

		watch_ui->watchTable->values_active(false);
		watch_ui->updateButton->deactivate();
		
	}
	
	Fl::wait();
	
}

void CommErrorMsg(int err) {
	
	switch(err) {
		case COMM_ERR_TIMEOUT:
			fl_message_title("Debugger Timeout");
			fl_message("Debug monitor stopped responding.");
			break;
			
		case COMM_ERR_RUNNING:
			fl_message_title("Program Still Running");
			fl_message("Close connection first or reboot target.");
			break;
			
		case COMM_ERR_NOREPLY:
			fl_message_title("Execute Error");
			fl_message("No start response from debug monitor.");
			break;
			
		case COMM_ERR_NODEBUG:
			fl_message_title("No Monitor Detected");
			fl_message("No compatible debug monitor detected.");
			break;
			
		case COMM_ERR_NOCONT:
			fl_message_title("Debugger Crashed");
			fl_message("Cannot continue execution.");
			break;
			
	}
	
}

void cb_IntervalUpdate(void* u);

void cb_commConnect(Fl_Button* w, void* u) {
	
	// Connects by pinging the target, to check for a debug monitor
	
	if( config.serial_dev.empty() )
	{
		fl_message_title("No Serial Device");
		fl_message("No serial device set.");
		return;
	}
	
	if( comms.serial.OpenPort(config.serial_dev.c_str())
		!= SerialClass::ERROR_NONE )
	{
		fl_message_title("Error Opening Port");
		fl_message("Cannot open serial port.");
		return;
	}
	
	if( comms.dbPing() )
	{
		comms.serial.ClosePort();
		
		CommErrorMsg(COMM_ERR_NODEBUG);
		return;
	}
	
	DebugControls(true);
	
	if( mem_ui->shown() )
	{
		Sleep(200);
		RefreshRemoteData();
	}
	
}

void cb_commDisconnect(Fl_Button* w, void* u) {
	
	comms.serial.ClosePort();
	DebugControls(false);
	
	watch_ui->watchTable->values_active(false);
	watch_ui->watchTable->update_values();
	watch_ui->updateButton->deactivate();
	
	Fl::remove_timeout(cb_IntervalUpdate);
	ui->updateToggle->uncheck();
	
}

void cb_Reboot(Fl_Button* w, void* u) {
	
	comms.dbReboot();
	ui->registers->SetLastState("Rebooted");
	
	comms.serial.ClosePort();
	DebugControls(false);
	
	Fl::remove_timeout(cb_IntervalUpdate);
	ui->updateToggle->uncheck();
	
}

void cb_Pause(Fl_Button* w, void* u) {
	
	comms.dbPause();
	
	db_wait_response = true;
	db_wait_timeout = 0;
	
	Fl::remove_timeout(cb_IntervalUpdate);
	
}

void cb_StepM(Fl_Menu_* w, void* u) { cb_Step((Fl_Button*)w, u); }
void cb_Step(Fl_Button* w, void* u) {
	
	if( !ui->stepButton->active() ) {
		return;
	}
	
	int ret = comms.dbStep();
	
	if( ret )
	{
		if( (ret&0xF0) == 0xC0 )
		{
			HandleCommsCode(ret);
			return;
		}
		
		if( ret < 0 )
		{
			CommErrorMsg(COMM_ERR_TIMEOUT);
			return;
		}
	}
	
	run_to_cursor = false;
	
	Sleep(100);
	
}

void cb_StepOverM(Fl_Menu_* w, void* u) { cb_StepOver((Fl_Button*)w, u); }
void cb_StepOver(Fl_Button* w, void* u) {
	
	if( !ui->stepOverButton->active() ) {
		return;
	}
	
	unsigned int pc = mips_GetNextPc(reg_buffer, true);

	#ifdef DEBUG
	std::cout << "Step to PC=" << std::hex << pc << std::dec << std::endl;
	#endif

	int ret = comms.dbRunTo(pc);
	
	if( ret )
	{
		if( (ret&0xF0) == 0xC0 )
		{
			HandleCommsCode(ret);
			return;
		}
		
		if( ret < 0 )
		{
			CommErrorMsg(COMM_ERR_TIMEOUT);
			return;
		}
	}
	
	run_to_cursor = false;
	
	Sleep(100);
	
}

void cb_StepOutM(Fl_Menu_* w, void* u) { cb_StepOut((Fl_Button*)w, u); }
void cb_StepOut(Fl_Button* w, void* u) {
	
	if( !ui->stepOutButton->active() ) {
		return;
	}
	
	#ifdef DEBUG
	std::cout << "Step out to ra=" << std::hex << reg_buffer[31] << std::dec << std::endl;
	#endif
	
	int ret = comms.dbRunTo(reg_buffer[PS_REG_ra]);
	
	if( ret )
	{
		if( (ret&0xF0) == 0xC0 )
		{
			HandleCommsCode(ret);
			return;
		}
		
		if( ret < 0 )
		{
			CommErrorMsg(COMM_ERR_TIMEOUT);
			return;
		}
	}
	
	Sleep(100);
	
}

void cb_ContinueM(Fl_Menu_* w, void* u) { cb_Continue((Fl_Button*)w, u); }
void cb_Continue(Fl_Button* w, void* u) {
	
	if( !ui->runButton->active() ) {
		return;
	}
	
	int ret = comms.dbContinue();
	
	if( ret )
	{
		if( ret == M_ERR_NOCONT )
		{
			CommErrorMsg(COMM_ERR_NOCONT);
			return;
		}
		else if( (ret&0xF0) == 0xC0 )
		{
			HandleCommsCode(ret);
			return;
		}
		else
		{
			CommErrorMsg(COMM_ERR_TIMEOUT);
		}
	}
	
	ui->registers->SetLastState("Running");
	ui->statusBar->SetStatus(nullptr);
	StepControls(false);
	DebugControls(true);
	
	if( ui->updateToggle->value() ) {
		Fl::add_timeout(config.update_interval, cb_IntervalUpdate);
	}
	
}

void cb_RefreshRegs(Fl_Button* w, void* u) {
	
	int ret = comms.dbRegisters(reg_buffer);
	
	if( ret ) {
		
		if( (ret&0xF0) == 0xC0 ) {
			HandleCommsCode(ret);
			return;
		}
		
		if( ret != M_ERR_OK ) {
			CommErrorMsg(COMM_ERR_TIMEOUT);
			return;
		}
		
	}
	
	ui->registers->UpdateRegs(reg_buffer);
	ui->disasm->SetPC(reg_buffer[PS_REG_epc], true);
	
	Sleep(100);
	
}

void cb_DataView(Fl_Light_Button* w, long userdata) {
	
	#ifdef DEBUG
	std::cout << "Mode = " << userdata << std::endl;
	#endif
	
	if( userdata == 1 ) {
		
		if( !RefreshRemoteCode() ) {
			return;
		}
		
		ui->codeRefresh->activate();
		ui->disasm->redraw();
		
	} else {
		
		ui->codeRefresh->deactivate();
		
	}
	
	ui->disasm->redraw();
	
}

void cb_IntervalUpdate(void *u) {
	
	UpdateViews();
	
	if( ui->updateToggle->value() ) {
		
		Fl::repeat_timeout(config.update_interval, cb_IntervalUpdate);
		
	}
	
}

void cb_AutoUpdate(Fl_Menu_ *w, void *u) {
	
	if( !comms.serial.IsOpen() ) {
		fl_message_title("Not connected");
		fl_message("Debugger not connected to a target.");
		return;
	}
	
	if( ui->updateToggle->value() )
	{
		Fl::add_timeout(config.update_interval, cb_IntervalUpdate);
	}
	else
	{
		Fl::remove_timeout(cb_IntervalUpdate);	
	}
	
}


OpcodeUI *op_ui;

void cb_PatchOpcode(Fl_Menu_ *w, void *u)
{
	if( Fl::focus() != ui->disasm )
	{
		fl_message_title("Disassembly not in focus");
		fl_message("Focus must be on the disassembly to patch opcodes.");
		return;
	}
	
	op_ui = new OpcodeUI();
	
	op_ui->show();
	
	while( op_ui->shown() )
	{
		Fl::wait();
	}
	
	delete op_ui;
	
}

void cb_OpcodeWrite(Fl_Return_Button *w, void *u)
{
	unsigned int opval;
	sscanf(op_ui->opcodeInput->value(), "%x", &opval);
	
	int ret = comms.dbSetWord(ui->disasm->GetPos()<<2, 2, opval);
	
	if( ret )
	{
		if( (ret&0xF0) == 0xC0 )
		{
			HandleCommsCode(ret);
			return;
		}
		CommErrorMsg(COMM_ERR_TIMEOUT);
		return;
	}
	
	op_ui->hide();
}

void cb_OpcodeCancel(Fl_Button *w, void *u)
{	
	op_ui->hide();
}

void cb_OpcodeInput(Fl_Input *w, void *u)
{
	unsigned int opval;
	char opcode[16];
	
	sscanf(w->value(), "%x", &opval);
	
	mips_Decode(opval, ui->disasm->GetPos()<<2, opcode, false);
	
	op_ui->opcode->value(opcode);
	
}