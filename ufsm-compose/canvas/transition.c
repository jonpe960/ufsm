#include <stdio.h>
#include <math.h>
#include <ufsm/model.h>

#include "canvas/view.h"

int transition_calc_begin_end_point(struct ufsmm_state *s,
                                    enum ufsmm_side side,
                                    double offset,
                                    double *x,
                                    double *y)
{
    double sx, sy, sw, sh;

    ufsmm_get_state_absolute_coords(s, &sx, &sy, &sw, &sh);

    if (s->kind == UFSMM_STATE_NORMAL) {
        switch (side) {
            case UFSMM_SIDE_LEFT:
                (*x) = sx;
                (*y) = sy + ((offset > sh)?sh:offset);
            break;
            case UFSMM_SIDE_RIGHT:
                (*x) = sx + sw;
                (*y) = sy + ((offset > sh)?sh:offset);
            break;
            case UFSMM_SIDE_TOP:
                (*x) = sx + ((offset > sw)?sw:offset);
                (*y) = sy;
            break;
            case UFSMM_SIDE_BOTTOM:
                (*x) = sx + ((offset > sw)?sw:offset);
                (*y) = sy + sh;
            break;
            default:
            break;
        }
    } else if ((s->kind == UFSMM_STATE_INIT) || (s->kind == UFSMM_STATE_FINAL) ||
               (s->kind == UFSMM_STATE_SHALLOW_HISTORY) ||
               (s->kind == UFSMM_STATE_DEEP_HISTORY)) {

        switch (side) {
            case UFSMM_SIDE_LEFT:
                (*x) = sx;
                (*y) = sy + sh/2;
            break;
            case UFSMM_SIDE_RIGHT:
                (*x) = sx + sw;
                (*y) = sy + sh/2;
            break;
            case UFSMM_SIDE_TOP:
                (*x) = sx + sw/2;
                (*y) = sy;
            break;
            case UFSMM_SIDE_BOTTOM:
                (*x) = sx + sw/2;
                (*y) = sy + sh;
            break;
            default:
            break;
        }
    }
}

static void render_selection_boxes(cairo_t *cr, struct ufsmm_transition *t)
{
    double fbx, fby;
    double rx, ry, rw, rh;
    struct ufsmm_vertice *v;
    double begin_x, begin_y, end_x, end_y;
    const double dashes[] = {10.0,  /* ink */
                             10.0};  /* skip */

    ufsmm_get_region_absolute_coords(t->source.state->parent_region,
                                    &rx, &ry, &rw, &rh);

    transition_calc_begin_end_point(t->source.state,
                         t->source.side,
                         t->source.offset,
                         &fbx, &fby);

    transition_calc_begin_end_point(t->dest.state,
                         t->dest.side,
                         t->dest.offset,
                         &end_x, &end_y);
    cairo_save(cr);
    ufsmm_color_set(cr, UFSMM_COLOR_ACCENT);
    cairo_rectangle (cr, fbx - 5, fby - 5, 10, 10);
    cairo_fill(cr);

    cairo_move_to (cr, fbx, fby);

    for (v = t->vertices; v; v = v->next) {
        cairo_rectangle (cr, v->x - 5, v->y - 5, 10, 10);
        cairo_fill(cr);
        cairo_move_to (cr, v->x, v->y);
        fbx = v->x;
        fby = v->y;
    }

    cairo_rectangle (cr, end_x - 5, end_y - 5, 10, 10);
    cairo_fill(cr);
    cairo_restore(cr);

    /* Dashed rectangle around the text block */
    double tx, ty, th, tw;
    tx = t->text_block_coords.x + rx;
    ty = t->text_block_coords.y + ry;
    th = t->text_block_coords.h;
    tw = t->text_block_coords.w;

    cairo_save(cr);
    ufsmm_color_set(cr, UFSMM_COLOR_ACCENT);
    cairo_set_dash (cr, dashes, 2, 0);
    cairo_set_line_width (cr, 2);
    cairo_rectangle (cr, tx, ty, tw, th);
    cairo_stroke (cr);
    cairo_restore (cr);
    /* Draw resize boxes for the text block */

    cairo_save(cr);
    ufsmm_color_set(cr, UFSMM_COLOR_ACCENT);
    cairo_rectangle (cr, tx - 5, ty - 5, 10, 10);
    cairo_rectangle (cr, tx + tw - 5, ty - 5, 10, 10);
    cairo_rectangle (cr, tx + tw - 5, ty + th - 5, 10, 10);
    cairo_rectangle (cr, tx - 5, ty + th - 5, 10, 10);
    cairo_fill(cr);
    cairo_restore(cr);
}

/*
static void render_transition_text(cairo_t *cr, struct ufsmm_transition *t)
{
    char text[1024];
    size_t text_pos = 0;
    cairo_text_extents_t extents;
    double rx, ry, rw, rh;

    cairo_save(cr);
    cairo_set_font_size (cr, 14);
    ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);

    ufsmm_get_region_absolute_coords(t->source.state->parent_region,
                                    &rx, &ry, &rw, &rh);

    enum ufsmm_state_kind source_kind = t->source.state->kind;
    if (source_kind == UFSMM_STATE_INIT) {
            snprintf(text, sizeof(text), "/ %s", "");
    } else {
        if (t->trigger) {
            text_pos = sprintf(&text[text_pos], "%s", t->trigger->name);
        } else {
            text_pos = sprintf(&text[text_pos], "trigger-less");
        }

        text_pos += sprintf(&text[text_pos], " [");
        for (struct ufsmm_action_ref *ar = t->guard; ar; ar = ar->next) {
            text_pos += sprintf(&text[text_pos], "%s%s",
                        ar->act->name, ar->next?", ":"");
        }
        text_pos += sprintf(&text[text_pos], "] / ");

        for (struct ufsmm_action_ref *ar = t->action; ar; ar = ar->next) {
            text_pos += sprintf(&text[text_pos], "%s%s",
                        ar->act->name, ar->next?", ":"");
        }
    }

    cairo_text_extents (cr, text, &extents);
    cairo_move_to (cr, rx + t->text_block_coords.x,
                       ry + t->text_block_coords.y + extents.height);
    cairo_show_text (cr, text);
    cairo_restore(cr);
}
*/

static void render_transition_text(cairo_t *cr, struct ufsmm_transition *t)
{
    size_t text_pos = 0;
    char text_buf[1024];
    cairo_text_extents_t extents;
    double rx, ry, rw, rh;
    double tx, ty, tw, th;
    unsigned int line_no = 0;
    const char *text_ptr = NULL;

    cairo_save(cr);
    cairo_set_font_size (cr, 14);
    ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);

    ufsmm_get_region_absolute_coords(t->source.state->parent_region,
                                    &rx, &ry, &rw, &rh);

    tx = t->text_block_coords.x;
    ty = t->text_block_coords.y;
    tw = t->text_block_coords.w;
    th = t->text_block_coords.h;

    double x = rx + tx;
    double y = ry + ty + 20;
    double x_space = tw;

    enum ufsmm_state_kind source_kind = t->source.state->kind;

    if (t->trigger) {
        text_ptr = t->trigger->name;
    } else {
        text_ptr = "trigger-less";
    }

    cairo_text_extents (cr, text_ptr, &extents);
    cairo_move_to (cr, x, y);
    cairo_show_text (cr, text_ptr);
    x_space -= extents.width;
    x += extents.width + 10;

    for (struct ufsmm_action_ref *ar = t->guard; ar; ar = ar->next) {
        if ((ar == t->guard) && ar->next)
            sprintf(text_buf, "[%s(), ", ar->act->name);
        else if ((ar == t->guard) && !ar->next)
            sprintf(text_buf, "[%s()]", ar->act->name);
        else if (!ar->next)
            sprintf(text_buf, "%s()]", ar->act->name);
        else
            sprintf(text_buf, "%s(), ", ar->act->name);

        cairo_text_extents (cr, text_buf, &extents);
        x_space -= (extents.width + 10);

        if (x_space < 10) {
            line_no++;
            x_space = tw;
            x = rx + tx;
        }

        y = ry + ty + 20 + 20 * line_no;

        ar->x = x;
        ar->y = y - extents.height / 2;
        ar->w = extents.width;
        ar->h = extents.height;

        if (!t->focus)
            ar->focus = false;

        cairo_move_to (cr, x, y);
        if (ar->focus)
            ufsmm_color_set(cr, UFSMM_COLOR_ACCENT);
        else
            ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);
        cairo_show_text (cr, text_buf);

        x += extents.width + 10;

    }

    for (struct ufsmm_action_ref *ar = t->action; ar; ar = ar->next) {
        if ((ar == t->action) && ar->next)
            sprintf(text_buf, " / %s(), ", ar->act->name);
        else if ((ar == t->action) && !ar->next)
            sprintf(text_buf, " / %s()", ar->act->name);
        else if (ar->next)
            sprintf(text_buf, "%s(), ", ar->act->name);
        else
            sprintf(text_buf, "%s()", ar->act->name);

        cairo_text_extents (cr, text_buf, &extents);
        x_space -= (extents.width + 10);

        if (x_space < 10) {
            line_no++;
            x_space = tw;
            x = rx + tx;
        }

        y = ry + ty + 20 + 20 * line_no;

        ar->x = x;
        ar->y = y - extents.height / 2;
        ar->w = extents.width;
        ar->h = extents.height;

        if (!t->focus)
            ar->focus = false;

        cairo_move_to (cr, x, y);

        if (ar->focus)
            ufsmm_color_set(cr, UFSMM_COLOR_ACCENT);
        else
            ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);
        cairo_show_text (cr, text_buf);

        x += extents.width + 10;

    }

    cairo_restore(cr);
}

int ufsmm_canvas_render_transition(cairo_t *cr,
                                  struct ufsmm_transition *transitions)
{
    double begin_x, begin_y, end_x, end_y;
    cairo_text_extents_t extents;
    struct ufsmm_vertice *v;
    double rx, ry, rw, rh;

    if (transitions == NULL)
        return UFSMM_OK;

    for (struct ufsmm_transition *t = transitions; t; t = t->next) {

        transition_calc_begin_end_point(t->source.state,
                             t->source.side,
                             t->source.offset,
                             &begin_x, &begin_y);

        transition_calc_begin_end_point(t->dest.state,
                             t->dest.side,
                             t->dest.offset,
                             &end_x, &end_y);

        ufsmm_get_region_absolute_coords(t->source.state->parent_region,
                                        &rx, &ry, &rw, &rh);

        cairo_save(cr);
        cairo_move_to (cr, begin_x, begin_y);
        cairo_set_line_width (cr, 2.0);

        for (v = t->vertices; v; v = v->next) {
            cairo_line_to(cr, v->x, v->y);
            ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);
            cairo_stroke (cr);
            cairo_move_to (cr, v->x, v->y);
            begin_x = v->x;
            begin_y = v->y;
        }

        cairo_line_to(cr, end_x, end_y);
        ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);
        cairo_set_line_width (cr, 2.0);
        cairo_stroke (cr);
        cairo_restore(cr);

        /* Draw arrow */
        double angle = atan2 (end_y - begin_y, end_x - begin_x) + M_PI;

        double x1 = end_x + 15 * cos(angle - 0.4);
        double y1 = end_y + 15 * sin(angle - 0.4);
        double x2 = end_x + 15 * cos(angle + 0.4);
        double y2 = end_y + 15 * sin(angle + 0.4);

        cairo_save(cr);

        ufsmm_color_set(cr, UFSMM_COLOR_NORMAL);
        cairo_new_sub_path(cr);
        cairo_move_to (cr, end_x, end_y);
        cairo_line_to(cr, x1, y1);
        cairo_line_to(cr, x2, y2);
        cairo_close_path(cr);
        cairo_fill(cr);
        cairo_restore(cr);

        render_transition_text(cr, t);

        /* Draw selection boxes if focused */
        if (t->focus)
            render_selection_boxes(cr, t);

    }

    return UFSMM_OK;
}