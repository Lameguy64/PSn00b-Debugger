// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef SettingsUI_h
#define SettingsUI_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
extern void cb_SettingsOkay(Fl_Button*, void*);
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Spinner.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Input_Choice.H>
extern void cb_SettingsExpToggle(Fl_Check_Button*, void*);
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Light_Button.H>
extern void cb_SettingsPatchToggle(Fl_Light_Button*, void*);
#include <FL/Fl_Input.H>
extern void cb_SettingsBrowsePatch(Fl_Button*, void*);

class SettingsUI : public Fl_Double_Window {
  void _SettingsUI();
public:
  SettingsUI(int X, int Y, int W, int H, const char *L = 0);
  SettingsUI(int W, int H, const char *L = 0);
  SettingsUI();
private:
  inline void cb_Cancel_i(Fl_Button*, void*);
  static void cb_Cancel(Fl_Button*, void*);
public:
  Fl_Check_Button *loadSymbolsCheck;
  Fl_Check_Button *autoMinimizeCheck;
  Fl_Spinner *updateIntValue;
  Fl_Choice *regsFontList;
  Fl_Spinner *regsFontSize;
  Fl_Choice *asmFontList;
  Fl_Spinner *asmFontSize;
  Fl_Input_Choice *serialChoice;
  Fl_Check_Button *bpcBreakToggle;
  Fl_Check_Button *ramExpEnable;
  Fl_Value_Input *ramExpSize;
  Fl_Check_Button *ram8mbEnable;
  Fl_Light_Button *patchFileToggle;
  Fl_Group *patchGroup;
  Fl_Input *patchFileInput;
};
#endif
