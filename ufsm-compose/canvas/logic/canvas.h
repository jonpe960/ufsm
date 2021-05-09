/* Automatically generated by uFSM version 1.0.0 */

#ifndef UFSM_MACHINE_canvas_H_
#define UFSM_MACHINE_canvas_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <ufsm/ufsm.h>

/* Triggers */
enum {
    eLMBDown,
    eLMBUp,
    eRMBDown,
    eRMBUp,
    eMotion,
    eEnablePan,
    eDisablePan,
    eKeyDown,
    eUserAbort,
    eToolError,
    eZoomIn,
    eZoomOut,
    eKey_r_down,
    eKey_esc_down,
    eKey_e_down,
    eKey_x_down,
    eKey_a_down,
    eMove,
    eDelete,
    eKey_s_down,
    eKey_t_down,
    eKey_delete_down,
    eKey_shift_down,
    eKey_shift_up,
    eKey_i_down,
    eKey_f_down,
    eSaveModel,
    eEnableScale,
    eDisableScale,
    eScrollUp,
    eScrollDown,
};

/* Entry action function prototypes */
void canvas_process_selection(void *context);
void canvas_show_tool_help(void *context);
void canvas_check_sresize_boxes(void *context);
void canvas_check_rresize_boxes(void *context);
void canvas_check_action_func(void *context);
void canvas_reset_focus(void *context);
void canvas_focus_state(void *context);
void canvas_focus_region(void *context);
void canvas_begin_mselect(void *context);
void canvas_focus_transition(void *context);
void canvas_check_transition_vertice(void *context);
void canvas_check_guard(void *context);
void canvas_check_action(void *context);
void canvas_focus_guard(void *context);
void canvas_focus_action(void *context);
void canvas_focus_entry(void *context);
void canvas_focus_exit(void *context);
void canvas_check_text_block(void *context);
void canvas_hide_state_hint(void *context);
void canvas_move_state_begin(void *context);

/* Exit action function prototypes */
void canvas_hide_tool_help(void *context);
void canvas_end_mselect(void *context);
void canvas_hide_state_hint(void *context);
void canvas_show_state_hint(void *context);
void canvas_hide_transition_hint(void *context);
void canvas_cleanup_transition(void *context);
void canvas_move_state_end(void *context);

/* Guard function prototypes */
bool canvas_region_selected(void *context);
bool canvas_state_selected(void *context);
bool canvas_state_resize_selected(void *context);
bool canvas_region_resize_selected(void *context);
bool canvas_state_entry_selected(void *context);
bool canvas_transition_selected(void *context);
bool canvas_transition_tvertice_selected(void *context);
bool canvas_transition_svertice_selected(void *context);
bool canvas_transition_text_block_selected(void *context);
bool canvas_transition_dvertice_selected(void *context);
bool canvas_guard_selected(void *context);
bool canvas_action_selected(void *context);
bool canvas_state_exit_selected(void *context);
bool canvas_textblock_resize_selected(void *context);
bool canvas_only_state_selected(void *context);
bool canvas_transition_vertice_selected(void *context);

/* Action function prototypes */
void canvas_select_root_region(void *context);
void canvas_update_selection(void *context);
void canvas_update_offset(void *context);
void canvas_zoom_in(void *context);
void canvas_zoom_out(void *context);
void canvas_move_state(void *context);
void canvas_resize_state(void *context);
void canvas_resize_region(void *context);
void canvas_reorder_entry_func(void *context);
void canvas_update_mselect(void *context);
void canvas_move_text_block(void *context);
void canvas_move_vertice(void *context);
void canvas_move_svertice(void *context);
void canvas_move_dvertice(void *context);
void canvas_reorder_exit_func(void *context);
void canvas_reorder_guard_func(void *context);
void canvas_reorder_action_func(void *context);
void canvas_resize_textblock(void *context);
void canvas_add_region(void *context);
void canvas_add_entry(void *context);
void canvas_add_exit(void *context);
void canvas_edit_state_name(void *context);
void canvas_edit_state_entry(void *context);
void canvas_edit_state_exit(void *context);
void canvas_translate_state(void *context);
void canvas_delete_entry(void *context);
void canvas_delete_exit(void *context);
void canvas_delete_state(void *context);
void canvas_new_state_set_scoords(void *context);
void canvas_show_state_hint(void *context);
void canvas_create_new_state(void *context);
void canvas_update_state_hint(void *context);
void canvas_create_transition_start(void *context);
void canvas_update_transition_hint(void *context);
void canvas_create_transition(void *context);
void canvas_transition_vdel_last(void *context);
void canvas_add_transition_vertice(void *context);
void canvas_create_init_state(void *context);
void canvas_create_final_state(void *context);
void canvas_save_model(void *context);
void canvas_inc_scale(void *context);
void canvas_store_offset(void *context);
void canvas_dec_scale(void *context);
void canvas_move_state_begin(void *context);
void canvas_resize_state_begin(void *context);

struct canvas_machine {
    struct ufsm_machine machine;
    struct ufsm_region_data region_data[9];
    struct ufsm_state_data state_data[60];
    void *stack_data[10];
    void *stack_data2[2];
};

/* Machine API */
int canvas_machine_initialize(struct canvas_machine *machine, void *ctx);
int canvas_machine_reset(struct canvas_machine *machine);
int canvas_machine_process(struct canvas_machine *machine, int ev);
#endif  // UFSM_MACHINE_canvas_H_
