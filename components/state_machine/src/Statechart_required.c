#include "Statechart_required.h"


extern void statechart_disable_alarm( Statechart* handle){ }
extern void statechart_activate_alarm( Statechart* handle){}
extern void statechart_display_unlocked( Statechart* handle){}
extern void statechart_display_locked( Statechart* handle){}
extern void statechart_new_password_input( Statechart* handle, const sc_integer password){}
extern sc_boolean statechart_verify_password( Statechart* handle, const sc_integer password){
    return false;
}
