// generated by Fast Light User Interface Designer (fluid) version 1.0110

#include "usbhxcfecfg_window.h"

extern void tick_usb(void *v);

void usbhxcfecfg_window::cb_OK_i(Fl_Button* o, void*) {
  ((Fl_Window*)(o->parent()))->hide();
}
void usbhxcfecfg_window::cb_OK(Fl_Button* o, void* v) {
  ((usbhxcfecfg_window*)(o->parent()->user_data()))->cb_OK_i(o,v);
}

usbhxcfecfg_window::usbhxcfecfg_window() {
  { window = new Fl_Double_Window(573, 252, "USB HxC Floppy Emulator settings and status");
    window->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(15, 17, 390, 63, "USB HxC Floppy Emulator status");
      o->box(FL_ENGRAVED_FRAME);
      o->labeltype(FL_EMBOSSED_LABEL);
      o->labelsize(12);
      o->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
      { strout_usbhfestatus = new Fl_Output(75, 42, 295, 25, "Status");
        strout_usbhfestatus->labelsize(12);
      } // Fl_Output* strout_usbhfestatus
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(15, 87, 390, 150, "USB statistics and settings");
      o->box(FL_ENGRAVED_FRAME);
      o->labeltype(FL_EMBOSSED_LABEL);
      o->labelsize(12);
      o->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
      { valout_packetsize = new Fl_Value_Output(108, 112, 82, 20, "Packet size");
        valout_packetsize->labelsize(12);
        valout_packetsize->textsize(12);
      } // Fl_Value_Output* valout_packetsize
      { strout_maxsettletime = new Fl_Output(108, 134, 82, 20, "Max settle time");
        strout_maxsettletime->labelsize(12);
        strout_maxsettletime->textsize(12);
      } // Fl_Output* strout_maxsettletime
      { strout_minsettletime = new Fl_Output(108, 155, 82, 20, "Min settle time");
        strout_minsettletime->labelsize(12);
        strout_minsettletime->textsize(12);
      } // Fl_Output* strout_minsettletime
      { slider_process_priority = new Fl_Slider(108, 177, 82, 20, "Process priority");
        slider_process_priority->type(5);
        slider_process_priority->labelsize(12);
        slider_process_priority->align(FL_ALIGN_LEFT);
      } // Fl_Slider* slider_process_priority
      { valout_synclost = new Fl_Value_Output(295, 178, 82, 20, "Sync lost");
        valout_synclost->labelsize(12);
        valout_synclost->textsize(12);
      } // Fl_Value_Output* valout_synclost
      { strout_packetsent = new Fl_Output(295, 156, 82, 20, "Packet sent");
        strout_packetsent->labelsize(12);
        strout_packetsent->textsize(12);
      } // Fl_Output* strout_packetsent
      { strout_datasent = new Fl_Output(295, 134, 82, 20, "Data sent");
        strout_datasent->labelsize(12);
        strout_datasent->textsize(12);
      } // Fl_Output* strout_datasent
      { strout_datathroughput = new Fl_Output(295, 112, 82, 20, "Data throughput");
        strout_datathroughput->labelsize(12);
        strout_datathroughput->textsize(12);
      } // Fl_Output* strout_datathroughput
      { new Fl_Button(295, 207, 85, 20, "Reset");
      } // Fl_Button* o
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(410, 17, 105, 109, "Drive ID setting");
      o->box(FL_ENGRAVED_FRAME);
      o->labeltype(FL_ENGRAVED_LABEL);
      o->labelsize(12);
      o->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
      { rbt_ds2 = new Fl_Round_Button(415, 59, 25, 25, "DS2");
        rbt_ds2->type(102);
        rbt_ds2->down_box(FL_ROUND_DOWN_BOX);
        rbt_ds2->labelsize(12);
      } // Fl_Round_Button* rbt_ds2
      { rbt_ds3 = new Fl_Round_Button(415, 73, 25, 25, "MTRON");
        rbt_ds3->type(102);
        rbt_ds3->down_box(FL_ROUND_DOWN_BOX);
        rbt_ds3->labelsize(12);
      } // Fl_Round_Button* rbt_ds3
      { chk_twistedcable = new Fl_Check_Button(415, 87, 25, 25, "Twisted cable");
        chk_twistedcable->down_box(FL_DOWN_BOX);
        chk_twistedcable->labelsize(12);
      } // Fl_Check_Button* chk_twistedcable
      { chk_disabledrive = new Fl_Check_Button(415, 101, 25, 25, "Disable drive");
        chk_disabledrive->down_box(FL_DOWN_BOX);
        chk_disabledrive->labelsize(12);
      } // Fl_Check_Button* chk_disabledrive
      { rbt_ds0 = new Fl_Round_Button(415, 34, 25, 25, "DS0");
        rbt_ds0->type(102);
        rbt_ds0->down_box(FL_ROUND_DOWN_BOX);
        rbt_ds0->labelsize(12);
      } // Fl_Round_Button* rbt_ds0
      { rbt_ds1 = new Fl_Round_Button(415, 47, 25, 25, "DS1");
        rbt_ds1->type(102);
        rbt_ds1->down_box(FL_ROUND_DOWN_BOX);
        rbt_ds1->labelsize(12);
      } // Fl_Round_Button* rbt_ds1
      o->end();
    } // Fl_Group* o
    { Fl_Group* o = new Fl_Group(410, 128, 105, 109, "Mode");
      o->box(FL_ENGRAVED_FRAME);
      o->labeltype(FL_ENGRAVED_LABEL);
      o->labelsize(12);
      o->align(FL_ALIGN_TOP_LEFT|FL_ALIGN_INSIDE);
      { chk_autoifmode = new Fl_Check_Button(415, 198, 25, 25, "Auto");
        chk_autoifmode->down_box(FL_DOWN_BOX);
        chk_autoifmode->labelsize(12);
      } // Fl_Check_Button* chk_autoifmode
      { chk_doublestep = new Fl_Check_Button(415, 212, 25, 25, "Double Step");
        chk_doublestep->down_box(FL_DOWN_BOX);
        chk_doublestep->labelsize(12);
      } // Fl_Check_Button* chk_doublestep
      { choice_ifmode = new Fl_Choice(415, 152, 95, 20);
        choice_ifmode->down_box(FL_BORDER_BOX);
        choice_ifmode->labelsize(12);
        choice_ifmode->textsize(12);
      } // Fl_Choice* choice_ifmode
      o->end();
    } // Fl_Group* o
    { new Fl_Button(520, 17, 45, 25, "Load");
    } // Fl_Button* o
    { new Fl_Button(520, 47, 45, 25, "Save");
    } // Fl_Button* o
    { Fl_Button* o = new Fl_Button(520, 77, 45, 25, "OK");
      o->callback((Fl_Callback*)cb_OK);
    } // Fl_Button* o
    window->end();
  } // Fl_Double_Window* window
  tick_usb(this);
}
