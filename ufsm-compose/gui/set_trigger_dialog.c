#include <gtk/gtk.h>
#include <ufsm/model.h>

enum
{
  COLUMN_MATCH_RATING,
  COLUMN_NAME,
  COLUMN_TRIGGER_REF,
  NUM_COLUMNS
};

struct ufsmm_trigger *selected_trigger;

static void input_changed(GtkEntry *entry, gpointer user_data)
{
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));
    GtkTreeSelection *selection = \
                gtk_tree_view_get_selection(GTK_TREE_VIEW(user_data));

    L_DEBUG("Input changed: %s", text);

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(user_data));
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter_first(model, &iter)) {
        while (&iter) {
            const char *iter_text;
            int match_rating = 0;
            gtk_tree_selection_unselect_iter(selection, &iter);
            gtk_tree_model_get(model, &iter, COLUMN_NAME, &iter_text, -1);

            if (strlen(text) > 1) {
                match_rating = strstr(iter_text, text)?1:0;
            } else {
                match_rating = 0;
            }

            gtk_list_store_set(GTK_LIST_STORE(model), &iter,
                                            COLUMN_MATCH_RATING,
                                            match_rating, -1);
            if (!gtk_tree_model_iter_next(model, &iter))
                break;
        }
    }

}

static gboolean input_key_cb(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    const char *text = gtk_entry_get_text(GTK_ENTRY(widget));

    if (event->keyval == GDK_KEY_Return) {
        if (strlen(text) > 0) {
            gtk_dialog_response(GTK_DIALOG(data), 1);
        }
    }
    return FALSE; /* Returning 'TRUE' prevents the entry widget from listening */
}

static void cell_data_func (GtkTreeViewColumn *col,
                            GtkCellRenderer   *renderer,
                            GtkTreeModel      *model,
                            GtkTreeIter       *iter,
                            gpointer           user_data)
{
    gchar *label;
    gchar *markuptxt;

    const char *search_text = gtk_entry_get_text(GTK_ENTRY(user_data));

    gtk_tree_model_get(model, iter, COLUMN_NAME, &label, -1);
    char *needle_begin = strstr(label, search_text);

    if (needle_begin && (strlen(search_text) > 1)) {
        size_t needle_len = strlen(search_text);
        char *needle_end = needle_begin + needle_len;
        markuptxt = g_malloc(strlen(label) + 10);
        memset(markuptxt, 0, strlen(label) + 10);
        memcpy(markuptxt, label, needle_end - label);
        sprintf(&markuptxt[needle_begin - label], "<b>%s</b>", search_text);
        memcpy(&markuptxt[(needle_begin - label) + needle_len + 7],
                needle_end, strlen(needle_end));
    } else {
        markuptxt = g_strdup_printf("%s", label);
    }

    g_object_set(renderer, "markup", markuptxt, NULL);
    g_free(markuptxt);
}

static gboolean view_selection_func(GtkTreeSelection *selection,
                               GtkTreeModel     *model,
                               GtkTreePath      *path,
                               gboolean          path_currently_selected,
                               gpointer          userdata)
{
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gchar *name;

        gtk_tree_model_get(model, &iter, COLUMN_NAME, &name, -1);

        if (!path_currently_selected) {
            gtk_tree_model_get(model, &iter, COLUMN_TRIGGER_REF,
                                &selected_trigger, -1);
        }

        g_free(name);
    }

    return TRUE; /* allow selection state to change */
}

static void list_row_activated_cb(GtkTreeView        *treeview,
                                   GtkTreePath        *path,
                                   GtkTreeViewColumn  *col,
                                   gpointer            userdata)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    model = gtk_tree_view_get_model(treeview);

    if (gtk_tree_model_get_iter(model, &iter, path)) {
       gchar *name;

       gtk_tree_model_get(model, &iter, COLUMN_NAME, &name, -1);
       gtk_tree_model_get(model, &iter, COLUMN_TRIGGER_REF, &selected_trigger, -1);

       g_print ("Selected trigger '%s'\n", name);
       gtk_dialog_response(GTK_DIALOG(userdata), GTK_RESPONSE_ACCEPT);
       g_free(name);
    }
}

int ufsm_set_trigger_dialog(GtkWindow *parent, struct ufsmm_model *model,
                            struct ufsmm_transition *transition)
{
    int rc;
    const char *msg;
    GtkWidget *dialog, *vbox, *content_area;
    GtkWidget *treeview;
    GtkDialogFlags flags;
    struct ufsmm_trigger *trigger;

    selected_trigger = NULL;

    flags = GTK_DIALOG_MODAL;
    dialog = gtk_dialog_new_with_buttons("Set trigger",
                                       parent,
                                       flags,
                                       "_OK",
                                       1,
                                       "_Cancel",
                                       GTK_RESPONSE_REJECT,
                                       NULL);

    content_area = gtk_dialog_get_content_area(GTK_DIALOG (dialog));

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 8);

    GtkWidget *input = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), input, FALSE, FALSE, 0);

    /* Create list */
    GtkListStore *store;
    GtkTreeIter iter;
    store = gtk_list_store_new (NUM_COLUMNS,
                                G_TYPE_UINT,
                                G_TYPE_STRING,
                                G_TYPE_POINTER);

    /* Addd 'trigger-less' trigger */
    gtk_list_store_append(store, &iter);
    gtk_list_store_set (store, &iter,
                        COLUMN_MATCH_RATING, 0,
                        COLUMN_NAME, "trigger-less",
                        COLUMN_TRIGGER_REF, NULL,
                        -1);

    struct ufsmm_trigger *t;
    TAILQ_FOREACH(t, &model->triggers, tailq) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set (store, &iter,
                            COLUMN_MATCH_RATING, 0,
                            COLUMN_NAME, t->name,
                            COLUMN_TRIGGER_REF, t,
                            -1);
    }

    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(store),
                                            COLUMN_MATCH_RATING,
                                            GTK_SORT_DESCENDING);

    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);

    treeview = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(treeview), FALSE);

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes ("Name",
                                                       renderer,
                                                       "text", COLUMN_NAME,
                                                       NULL);
    gtk_tree_view_column_set_cell_data_func(column, renderer,
                                            cell_data_func, input, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(treeview), column);

    gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
    gtk_widget_set_size_request(scrolled_window, 640, 480);

    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));

    gtk_tree_selection_set_select_function(selection, view_selection_func,
                                            input, NULL);

    /* Connect signals */
    gtk_widget_show_all(vbox);

    g_signal_connect(G_OBJECT(input), "changed",
                                      G_CALLBACK(input_changed),
                                      G_OBJECT(treeview));

    g_signal_connect(G_OBJECT(input), "key_press_event",
                                      G_CALLBACK(input_key_cb),
                                      G_OBJECT(dialog));

    g_signal_connect(treeview, "row-activated",
                               G_CALLBACK(list_row_activated_cb),
                               G_OBJECT(dialog));

    int result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_ACCEPT) {
        rc = ufsmm_transition_set_trigger(model, transition,
                                            selected_trigger);
    } else if (result == 1) { /* Create new action */
        const char *new_name = gtk_entry_get_text(GTK_ENTRY(input));
        rc = ufsmm_model_add_trigger(model, new_name, &selected_trigger);

        if (rc != UFSMM_OK)
            goto err_out;

        rc = ufsmm_transition_set_trigger(model, transition,
                                            selected_trigger);
    } else {
        rc = -1;
    }

err_out:
    gtk_widget_destroy (dialog);

    return rc;
}
