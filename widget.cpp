#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
extern "C" {
#include <glib-object.h>
#include <glib.h>
#include <gio/gio.h>
#include <gobject/gparamspecs.h>
#include <glib/gi18n.h>
}
#include <XdgIcon>
#include <XdgDesktopFile>
#include "utils.h"
#include <QHBoxLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QStringList>
#define MATE_DESKTOP_USE_UNSTABLE_API
#define VERSION "1.12.1"
#define GVC_DIALOG_DBUS_NAME "org.mate.VolumeControl"
#define KEY_SOUNDS_SCHEMA   "org.mate.sound"
#define GVC_SOUND_SOUND    (xmlChar *) "sound"
#define GVC_SOUND_NAME     (xmlChar *) "name"
#define GVC_SOUND_FILENAME (xmlChar *) "filename"
#define SOUND_SET_DIR "/usr/share/ukui-media/sounds"

guint appnum = 0;
typedef enum {
    BALANCE_TYPE_RL,
    BALANCE_TYPE_FR,
    BALANCE_TYPE_LFE,
} GvcBalanceType;

enum {
    SOUND_TYPE_UNSET,
    SOUND_TYPE_OFF,
    SOUND_TYPE_DEFAULT_FROM_THEME,
    SOUND_TYPE_BUILTIN,
    SOUND_TYPE_CUSTOM
};

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    if (mate_mixer_init() == FALSE) {
        qDebug() << "libmatemixer initialization failed, exiting";
    }

    ui->appVolumeTableView->setSelectionMode(QAbstractItemView::NoSelection);
    soundlist = new QStringList;
    theme_name_list = new QStringList;
    theme_display_name_list = new QStringList;
    device_name_list = new QStringList;
    output_stream_list = new QStringList;
    input_stream_list = new QStringList;
    app_volume_list = new QStringList;
    stream_control_list = new QStringList;
    //应用音量显示标签
    app_display_label = new QLabel(ui->appVolumeTableView);
    app_display_label->setText(tr("No application is currently playing or recording audio"));

    //创建ItemModel 添加应用音量
    standItemModel = new QStandardItemModel();
    standItemModel->setColumnCount(4);

    //创建context
    context = mate_mixer_context_new();
//    gboolean ok =  mate_mixer_context_set_backend_type(context,MATE_MIXER_BACKEND_ALSA);

    mate_mixer_context_set_app_name (context,_("Volume Control"));//设置app名
    mate_mixer_context_set_app_id(context, GVC_DIALOG_DBUS_NAME);
    mate_mixer_context_set_app_version(context,VERSION);
    mate_mixer_context_set_app_icon(context,"multimedia-volume-control");

    //打开context
    if G_UNLIKELY (mate_mixer_context_open(context) == FALSE) {
        qDebug() << "open context fail";
        g_warning ("Failed to connect to a sound system**********************");
    }

    g_param_spec_object ("context",
                        "Context",
                        "MateMixer context",
                        MATE_MIXER_TYPE_CONTEXT,
                        (GParamFlags)(G_PARAM_READWRITE |
                        G_PARAM_CONSTRUCT_ONLY |
                        G_PARAM_STATIC_STRINGS));

    if (mate_mixer_context_get_state (context) == MATE_MIXER_STATE_CONNECTING) {
//         popup_id = g_timeout_add_seconds (DIALOG_POPUP_TIMEOUT,
//                                           dialog_popup_timeout,
//                                           NULL);

    }

    context_set_property(this);
    g_signal_connect (G_OBJECT (context),
                     "notify::state",
                     G_CALLBACK (on_context_state_notify),
                     this);

    //隐藏QTableView表格和网格线
    ui->appVolumeTableView->setShowGrid(false);
    ui->appVolumeTableView->horizontalHeader()->setVisible(false);// 水平不可见
    ui->appVolumeTableView->verticalHeader()->setVisible(false);// 垂直不可见
    //设置QTableView行高
    ui->appVolumeTableView->verticalHeader()->setDefaultSectionSize(40);
    ui->appVolumeTableView->horizontalHeader()->setFixedWidth(180);
    //设置滑动条的最大值为100
    ui->ipVolumeSlider->setMaximum(100);
    ui->opVolumeSlider->setMaximum(100);
    ui->opBalanceSlider->setMaximum(100);
    ui->opBalanceSlider->setMinimum(-100);
    ui->opBalanceSlider->setSingleStep(100);
    ui->inputLevelSlider->setMaximum(100);

    ui->appVolumeTableView->setColumnWidth(2,180);

    //设置声音主题
    //获取声音gsettings值
    sound_settings = g_settings_new (KEY_SOUNDS_SCHEMA);

    g_signal_connect (G_OBJECT (sound_settings),
                             "changed",
                             G_CALLBACK (on_key_changed),
                             this);
    setup_theme_selector(this);
    update_theme(this);

    //报警声音,从指定路径获取报警声音文件
    populate_model_from_dir(this,SOUND_SET_DIR);

    //点击报警音量时播放报警声音
    connect(this->ui->shutdownCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(combox_index_changed_slot(int)));
    connect(this->ui->lagoutCombobox ,SIGNAL(currentIndexChanged(int)),this,SLOT(combox_index_changed_slot(int)));
    connect(this->ui->soundThemeCombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(theme_combox_index_changed_slot(int)));

//    setWindowFlag(Qt::Tool);
}

/*
 * context状态通知
*/
void Widget::on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,Widget	*w)
{
    MateMixerState state = mate_mixer_context_get_state (context);
    list_device(w,context);
    if (state == MATE_MIXER_STATE_READY) {

        update_icon_output(w);
        update_icon_input(w);
//                remove_warning_dialog ();
//                context_ready (context, app);
    }
    else if (state == MATE_MIXER_STATE_FAILED) {

//                GtkWidget *dialog;

//                remove_warning_dialog ();

//                dialog = gtk_message_dialog_new (GTK_WINDOW (app_dialog),
//                                                 0,
//                                                 GTK_MESSAGE_ERROR,
//                                                 GTK_BUTTONS_CLOSE,
//                                                 _("Sound system is not available"));

//                g_signal_connect (G_OBJECT (dialog),
//                                  "response",
//                                  G_CALLBACK (on_dialog_response),
//                                  GINT_TO_POINTER (TRUE));
//                g_signal_connect (G_OBJECT (dialog),
//                                  "close",
//                                  G_CALLBACK (on_dialog_close),
//                                  GINT_TO_POINTER (TRUE));

//                gtk_widget_show (dialog);
    }
    qDebug() << "设备列表为" << w->device_name_list->at(0) << w->device_name_list->size() << "stream size" << w->output_stream_list->at(0);
    //点击输出设备
    connect(w->ui->outputDeviceCombobox,SIGNAL(currentIndexChanged(int)),w,SLOT(output_device_combox_index_changed_slot(int)));
    //点击输入设备
    connect(w->ui->inputDeviceCombobox,SIGNAL(currentIndexChanged(int)),w,SLOT(input_device_combox_index_changed_slot(int)));

}

/*
    context 存储control增加
*/
void Widget::on_context_stored_control_added (MateMixerContext *context,const gchar      *name,Widget *w)
{
    MateMixerStreamControl *control;
    MateMixerStreamControlMediaRole media_role;

    control = MATE_MIXER_STREAM_CONTROL (mate_mixer_context_get_stored_control (context, name));
    if (G_UNLIKELY (control == nullptr))
        return;

    media_role = mate_mixer_stream_control_get_media_role (control);

    if (media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT)
        bar_set_stream_control (w, control);
}


/*
    当其他设备插入时添加这个stream
*/
void Widget::on_context_stream_added (MateMixerContext *context,const gchar *name,Widget *w)
{
    MateMixerStream *stream;
    MateMixerDirection direction;
//        GtkWidget         *bar;
    stream = mate_mixer_context_get_stream (context, name);
    qDebug() << "context stream 添加" << name;
    if (G_UNLIKELY (stream == nullptr))
        return;
    direction = mate_mixer_stream_get_direction (stream);

    /* If the newly added stream belongs to the currently selected device and
     * the test button is hidden, this stream may be the one to allow the
     * sound test and therefore we may need to enable the button */
    if (/*dialog->priv->hw_profile_combo != nullptr && */direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerDevice *device1;
        MateMixerDevice *device2;

        device1 = mate_mixer_stream_get_device (stream);
//                device2 = g_object_get_data (G_OBJECT (dialog->priv->hw_profile_combo),
//                                             "device");

        if (device1 == device2) {
            gboolean show_button;

//                        g_object_get (G_OBJECT (dialog->priv->hw_profile_combo),
//                                      "show-button", &show_button,
//                                      NULL);

//                        if (show_button == FALSE)

//                                updat为auto_nulle_device_test_visibility (dialog);
        }
    }

//        bar = g_hash_table_lookup (dialog->priv->bars, name);
//        if (G_UNLIKELY (bar != nullptr))
//                return;

    add_stream (w, stream,context);
}

/*
列出设备
*/
void Widget::list_device(Widget *w,MateMixerContext *context)
{
    const GList *list;
    const GList *stream_list;

    list = mate_mixer_context_list_streams (context);

    while (list != nullptr) {
        add_stream (w, MATE_MIXER_STREAM (list->data),context);
        MateMixerStream *s = MATE_MIXER_STREAM(list->data);
        const gchar *stream_name = mate_mixer_stream_get_name(s);

        MateMixerDirection direction = mate_mixer_stream_get_direction(s);
        if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
            w->output_stream_list->append(stream_name);
            qDebug() << "输出stream 名为" << mate_mixer_stream_get_name(s);
        }
        else if (direction == MATE_MIXER_DIRECTION_INPUT) {
            w->input_stream_list->append(stream_name);
            qDebug() << "输入stream 名为" << mate_mixer_stream_get_name(s);
        }
        list = list->next;
    }

    list = mate_mixer_context_list_devices (context);

    while (list != nullptr) {
//                    add_device (self, MATE_MIXER_DEVICE (list->data));
        QString str =  mate_mixer_device_get_label(MATE_MIXER_DEVICE (list->data));

        const gchar *dis_name = mate_mixer_device_get_name(MATE_MIXER_DEVICE (list->data));
        w->device_name_list->append(dis_name);
        w->ui->inputDeviceCombobox->addItem(str);
        w->ui->outputDeviceCombobox->addItem(str);
        qDebug() << "设备名为" << str << dis_name;
//        stream_list = mate_mixer_device_list_streams(MATE_MIXER_DEVICE (list->data));
//        while (stream_list != nullptr) {
//            MateMixerStream *s = MATE_MIXER_STREAM(stream_list->data);
//            const gchar *stream_name = mate_mixer_stream_get_name(s);

//            MateMixerDirection direction = mate_mixer_stream_get_direction(s);
//            if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
//                w->output_stream_list->append(stream_name);
//                qDebug() << "输出stream 名为" << mate_mixer_stream_get_name(s);
//            }
//            else if (direction == MATE_MIXER_DIRECTION_INPUT) {
//                w->input_stream_list->append(stream_name);
//                qDebug() << "输入stream 名为" << mate_mixer_stream_get_name(s);
//            }
//            stream_list = stream_list->next;
//        }


        list = list->next;
    }

}

void Widget::add_stream (Widget *w, MateMixerStream *stream,MateMixerContext *context)
{
//        GtkTreeModel      *model = nullptr;
//        GtkTreeIter        iter;

    const gchar *speakers = nullptr;
    const GList *controls;
    gboolean is_default = FALSE;
    MateMixerDirection direction;
    utils utils ;
    direction = mate_mixer_stream_get_direction (stream);
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        MateMixerStream *input;
        input = mate_mixer_context_get_default_input_stream (context);
//        MateMixerDevice *ipDevice = mate_mixer_stream_get_device(input);

//        MateMixerDeviceSwitch *ipSwitch = mate_mixer_device_get_switch(ipDevice,"profile");

        if (stream == input) {
            bar_set_stream (w, stream);

//                        update_input_settings (dialog);
            is_default = TRUE;
        }
//                model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->input_treeview));
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        MateMixerStream        *output;
        MateMixerStreamControl *control;
        output = mate_mixer_context_get_default_output_stream (context);
//        MateMixerDevice *device = mate_mixer_stream_get_device(output);
//        MateMixerDeviceSwitch *opSwitch = mate_mixer_device_get_switch(device,"profile");
        control = mate_mixer_stream_get_default_control (stream);

        if (stream == output) {
            update_output_settings(w,control);
            qDebug() << "stream is default";
            bar_set_stream (w, stream);

            is_default = TRUE;
        }
//                model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->output_treeview));

        if (G_LIKELY (control != nullptr)) {
            speakers = utils.gvc_channel_map_to_pretty_string (control);

        }
    }

    controls = mate_mixer_stream_list_controls (stream);

    while (controls != nullptr) {
        MateMixerStreamControl    *control = MATE_MIXER_STREAM_CONTROL (controls->data);
        MateMixerStreamControlRole role;

        role = mate_mixer_stream_control_get_role (control);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            add_application_control (w, control);
        }
        controls = controls->next;
    }

//        if (/*model != nullptr*/ 1) {
//                const gchar *name;
//                const gchar *label;

//                name  = mate_mixer_stream_get_name (stream);
//                label = mate_mixer_stream_get_label (stream);
//            qDebug() << "++++++++" << name << label;
//                gtk_list_store_append (GTK_LIST_STORE (model), &iter);
//                gtk_list_store_set (GTK_LIST_STORE (model),
//                                    &iter,
//                                    NAME_COLUMN, name,
//                                    LABEL_COLUMN, label,
//                                    ACTIVE_COLUMN, is_default,
//                                    SPEAKERS_COLUMN, speakers,
//                                    -1);
//        }
    // XXX find a way to disconnect when removed
    g_signal_connect (G_OBJECT (stream),
                      "control-added",
                      G_CALLBACK (on_stream_control_added),
                      w);
    g_signal_connect (G_OBJECT (stream),
                      "control-removed",
                      G_CALLBACK (on_stream_control_removed),
                      w);
}

/*
    添加应用音量控制
*/
void Widget::add_application_control (Widget *w, MateMixerStreamControl *control)
{
    MateMixerStream *stream;
    MateMixerStreamControlMediaRole media_role;
    MateMixerAppInfo *info;
    guint app_count;
    MateMixerDirection direction = MATE_MIXER_DIRECTION_UNKNOWN;
//        GtkWidget                      *bar;
    const gchar *app_id;
    const gchar *app_name;
    const gchar *app_icon;
    appnum++;
    app_count = appnum;
    guint volume = mate_mixer_stream_control_get_volume(control);

    media_role = mate_mixer_stream_control_get_media_role (control);

    /* Add stream to the applications page, but make sure the stream qualifies
     * for the inclusion */
    info = mate_mixer_stream_control_get_app_info (control);
    if (info == nullptr)
        return;

    /* Skip streams with roles we don't care about */
    if (media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_EVENT ||
        media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_TEST ||
        media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_ABSTRACT ||
        media_role == MATE_MIXER_STREAM_CONTROL_MEDIA_ROLE_FILTER)
            return;

    app_id = mate_mixer_app_info_get_id (info);

    /* These applications may have associated streams because they do peak
     * level monitoring, skip these too */
    if (!g_strcmp0 (app_id, "org.mate.VolumeControl") ||
        !g_strcmp0 (app_id, "org.gnome.VolumeControl") ||
        !g_strcmp0 (app_id, "org.PulseAudio.pavucontrol"))
            return;

    QString app_icon_name = mate_mixer_app_info_get_icon(info);

    app_name = mate_mixer_app_info_get_name (info);

//    for (app_count=0;app_count<appnum;app_count++) {
        w->standItemModel->setRowCount(appnum);

        w->ui->appVolumeTableView->setModel(w->standItemModel);
        //设置QTableView每行的宽度
        w->ui->appVolumeTableView->setColumnWidth(0,100);
        w->ui->appVolumeTableView->setColumnWidth(1,40);
        w->ui->appVolumeTableView->setColumnWidth(2,180);
        w->ui->appVolumeTableView->setColumnWidth(3,40);
        add_app_to_tableview(w,appnum,w->standItemModel,app_name,app_icon_name,control);

//    }
    w->ui->appVolumeTableView->show();
    if (app_name == nullptr)
        app_name = mate_mixer_stream_control_get_label (control);
    if (app_name == nullptr)
        app_name = mate_mixer_stream_control_get_name (control);
    if (G_UNLIKELY (app_name == nullptr))
        return;

//    bar = create_bar (dialog, FALSE, FALSE);

//    g_object_set (G_OBJECT (bar),
//                  "show-marks", FALSE,
//                  "extended", FALSE,
//                  NULL);

    /* By default channel bars use speaker icons, use microphone icons
     * instead for recording applications */
    stream = mate_mixer_stream_control_get_stream (control);
    if (stream != nullptr)
        direction = mate_mixer_stream_get_direction (stream);

    if (direction == MATE_MIXER_DIRECTION_INPUT) {

//                g_object_set (G_OBJECT (bar),
//                              "low-icon-name", "audio-input-microphone-low",
//                              "high-icon-name", "audio-input-microphone-high",
//                              NULL);
    }
    app_icon = mate_mixer_app_info_get_icon (info);
    if (app_icon == nullptr) {
        if (direction == MATE_MIXER_DIRECTION_INPUT)
            app_icon = "audio-input-microphone";
        else
            app_icon = "applications-multimedia";
    }

//        gvc_channel_bar_set_name (GVC_CHANNEL_BAR (bar), app_name);
//        gvc_channel_bar_set_icon_name (GVC_CHANNEL_BAR (bar), app_icon);

//        gtk_box_pack_start (GTK_BOX (dialog->priv->applications_box),
//                            bar,
//                            FALSE, FALSE, 12);

        bar_set_stream_control (w, control);

//        gtk_widget_hide (dialog->priv->no_apps_label);
//        gtk_widget_show (bar);
}

void Widget::on_stream_control_added (MateMixerStream *stream,const gchar *name,Widget *w)
{
    MateMixerStreamControl    *control;
    MateMixerStreamControlRole role;
    qDebug() << "add stream control" << name;
    w->stream_control_list->append(name);
    control = mate_mixer_stream_get_control (stream, name);
    if G_UNLIKELY (control == nullptr)
        return;

    role = mate_mixer_stream_control_get_role (control);
    if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
        add_application_control (w, control);
    }
}

/*
    移除control
*/
void Widget::on_stream_control_removed (MateMixerStream *stream,const gchar *name,Widget *w)
{
    MateMixerStreamControl *control;
    qDebug() << "stream control remove" << name;
    w->stream_control_list->append(name);
//        control = gvc_channel_bar_get_control (GVC_CHANNEL_BAR (dialog->priv->input_bar));
//    if (control != nullptr) {
//        const gchar *input_name = mate_mixer_stream_control_get_name (control);

//        if (strcmp (name, input_name) == 0) {
//                // XXX probably can't even happen, but handle it somehow
//            return;
//        }
//    }

//        control = gvc_channel_bar_get_control (GVC_CHANNEL_BAR (dialog->priv->output_bar));
//    if (control != nullptr) {
//        const gchar *input_name = mate_mixer_stream_control_get_name (control);

//        if (strcmp (name, input_name) == 0) {
//                // XXX probably can't even happen, but handle it somehow
//            return;
//        }
//    }

    /* No way to be sure that it is an application control, but we don't have
     * any other than application bars that could match the name */
    remove_application_control (w, name);
}

void Widget::remove_application_control (Widget *w,const gchar *name)
{
//        GtkWidget *bar;

//        bar = g_hash_table_lookup (dialog->priv->bars, name);
//        if (G_UNLIKELY (bar == nullptr))
//                return;

    g_debug ("Removing application stream %s", name);
        /* We could call bar_set_stream_control here, but that would pointlessly
         * invalidate the channel bar, so just remove it ourselves */
//        g_hash_table_remove (dialog->priv->bars, name);

//        gtk_container_remove (GTK_CONTAINER (gtk_widget_get_parent (bar)), bar);
    int index = w->app_volume_list->indexOf(name);
    int i = w->stream_control_list->indexOf(name);

    w->stream_control_list->removeOne(name);

//    MateMixerStream *s =  mate_mixer_stream_control_get_stream(control);
//    MateMixerAppInfo *info = mate_mixer_stream_control_get_app_info(control);
//    const gchar *app_name = mate_mixer_app_info_get_name(info);
    qDebug() << "移除stream control" << i << "移除应用app_name  name"  << name << index << w->stream_control_list->size() ;
//    w->app_volume_list->removeAt(index);
//    w->standItemModel->removeRows(0,i+1);
    if (appnum <= 0) {
        g_warn_if_reached ();
        appnum = 1;
    }
    appnum--;
    if (appnum <= 0)
        w->app_display_label->show();
    else
        w->app_display_label->hide();

}

void Widget::add_app_to_tableview(Widget *w,int appnum, QStandardItemModel *standItemModel,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control)
{
    //获取应用静音状态及音量
    int volume = 0;
    gboolean is_mute = false;
    gdouble normal = 0.0;
    is_mute = mate_mixer_stream_control_get_mute(control);
    volume = mate_mixer_stream_control_get_volume(control);
    normal = mate_mixer_stream_control_get_normal_volume(control);
//    qDebug() << "转换前" << volume << normal << app_name << endl;
//    if(volume <= normal / 100 * 60.0)
//        volume = volume / 3.0;
//    else if(volume > normal / 100.0 * 80.0)
//        volume = normal / 100.0 * 40.0 +(volume - normal / 100.0 * 80.0) * 3.0;
//    else
//        volume = volume - normal / 100.0 * 40.0;
    int display_volume = 100 * volume / normal;

//    qDebug() <<"转换后" << volume << normal << display_volume << app_name << endl;

    //设置应用的图标
    QString iconName = "/usr/share/applications/";
    iconName.append(app_icon_name);
    iconName.append(".desktop");
    XdgDesktopFile xdg;
    xdg.load(iconName);
    QIcon i=xdg.icon();
    GError **error = nullptr;
    GKeyFileFlags flags = G_KEY_FILE_NONE;
    GKeyFile *keyflie = g_key_file_new();
    QByteArray fpbyte = iconName.toLocal8Bit();
    char *filepath = "/usr/share/applications";//fpbyte.data();
    g_key_file_load_from_file(keyflie,iconName.toLocal8Bit(),flags,error);
    char *icon_str = g_key_file_get_locale_string(keyflie,"Desktop Entry","Icon",nullptr,nullptr);
    qDebug() << "+++++++++++++ icon" << icon_str << "file path"<< filepath;
    QIcon icon = QIcon::fromTheme(QString::fromLocal8Bit(icon_str));
    w->app_volume_list->append(app_icon_name);

    w->appLabel = new QLabel(w->ui->appVolumeTableView);
    w->appIconBtn = new QPushButton(w->ui->appVolumeTableView);
    w->appVolumeLabel = new QLabel(w->ui->appVolumeTableView);
    w->appSlider = new AudioSlider(w->ui->appVolumeTableView);
    w->appSlider->setOrientation(Qt::Horizontal);

    w->appIconBtn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    w->appIconBtn->setIcon(icon);
    w->appIconBtn->setFlat(true);
    w->appIconBtn->setEnabled(true);

    w->appSlider->setMaximum(100);
    w->appSlider->setFixedSize(178,20);
    w->ui->appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,0),w->appLabel);
    w->ui->appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,1),w->appIconBtn);
    w->ui->appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,2),w->appSlider);
    w->ui->appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,3),w->appVolumeLabel);

    QString appSliderStr = app_name;
    QString appLabelStr = app_name;
    QString appVolumeLabelStr = app_name;

    appSliderStr.append("Slider");
    appLabelStr.append("Label");
    appVolumeLabelStr.append("VolumeLabel");
    w->appSlider->setObjectName(appSliderStr);
    w->appLabel->setObjectName(appLabelStr);
    w->appVolumeLabel->setObjectName(appVolumeLabelStr);
    //设置label 和滑动条的值
    w->appLabel->setText(app_name);

    w->appVolumeLabel->setNum(display_volume);
    w->appSlider->setValue(display_volume);
//    qDebug() << "应用静音状态及音量为" << is_mute <<volume ;
//    connect(w->appSlider,SIGNAL(valueChanged(int)),w,SLOT(app_slider_changed_slot(int)));

    /*滑动条控制应用音量*/
    connect(w->appSlider,&QSlider::valueChanged,[=](int value){
        QSlider *s = w->findChild<QSlider*>(appSliderStr);
        s->setValue(value);
        QLabel *l = w->findChild<QLabel*>(appVolumeLabelStr);
        l->setNum(value);

        int v = value*65536/100 + 0.5;
        mate_mixer_stream_control_set_volume(control,(int)v);
//        qDebug() << "滚动滑动条" << value << appVolumeLabelStr;
    });
    /*应用音量同步*/
    g_signal_connect (G_OBJECT (control),
                     "notify::volume",
                     G_CALLBACK (update_app_volume),
                     w);

    connect(w,&Widget::app_volume_changed,[=](bool is_mute,int volume,const gchar *app_name){
//        qDebug() << "获取的应用音量及静音状态" << volume << is_mute << appLabelStr;

    //        w->appSlider->setValue(volume);
        QString slider_str = app_name;
        slider_str.append("Slider");
        QSlider *s = w->findChild<QSlider*>(slider_str);
        s->setValue(volume);
    });

    if (appnum <= 0)
        w->app_display_label->show();
    else
        w->app_display_label->hide();
}

/*
    应用音量滑动条滚动事件
*/
void Widget::app_slider_changed_slot(int volume)
{
//    qDebug() << "滚动滑动条" << volume;
    mate_mixer_stream_control_set_volume;
//    qDebug() << mate_mixer_stream_control_get_volume;
//    if (volume > 0) {
//        mate_mixer_stream_control_set_mute(control,FALSE);
//        w->appVolumeLabel->setNum(volume);
//        mate_mixer_stream_control_set_volume(w->control,volume*65536/100);
//    }
//    else if (volume <= 0) {
//        mate_mixer_stream_control_set_mute(w->control,TRUE);
//        mate_mixer_stream_control_set_volume(w->control,0);
//        w->appVolumeLabel->setNum(0);
//    }

}

/*
    同步应用音量
*/
void Widget::update_app_volume(MateMixerStreamControl *control, GParamSpec *pspec, Widget *w)
{
    Q_UNUSED(pspec);

    guint value = mate_mixer_stream_control_get_volume(control);
    guint volume ;
    volume = guint(value*100/65536.0+0.5);
    bool is_mute = mate_mixer_stream_control_get_mute(control);
    MateMixerStreamControlFlags control_flags = mate_mixer_stream_control_get_flags(control);
    MateMixerAppInfo *info = mate_mixer_stream_control_get_app_info(control);
    const gchar *app_name = mate_mixer_app_info_get_name(info);
    Q_EMIT w->app_volume_changed(is_mute,volume,app_name);
//    qDebug() << "发送信号同步音量值" << is_mute <<volume ;

    //静音可读并且处于静音
    if ((control_flags & MATE_MIXER_STREAM_CONTROL_MUTE_WRITABLE) ) {
    }
    if (control_flags & MATE_MIXER_STREAM_CONTROL_VOLUME_WRITABLE) {
        //设置滑动条的值
//        Q_EMIT->emitVolume(volume);
    }
}

void Widget::app_volume_changed_slot(bool is_mute,int volume,const gchar *app_name)
{

}

/*
    连接context，处理不同信号
*/
void Widget::set_context(Widget *w,MateMixerContext *context)
{
    g_signal_connect (G_OBJECT (context),
                      "stream-added",
                      G_CALLBACK (on_context_stream_added),
                      w);

    g_signal_connect (G_OBJECT (context),
                    "stream-removed",
                    G_CALLBACK (on_context_stream_removed),
                    w);

    g_signal_connect (G_OBJECT (context),
                    "device-added",
                    G_CALLBACK (on_context_device_added),
                    w);
    g_signal_connect (G_OBJECT (context),
                    "device-removed",
                    G_CALLBACK (on_context_device_removed),
                    w);

    g_signal_connect (G_OBJECT (context),
                    "notify::default-input-stream",
                    G_CALLBACK (on_context_default_input_stream_notify),
                    w);
    g_signal_connect (G_OBJECT (context),
                    "notify::default-output-stream",
                    G_CALLBACK (on_context_default_output_stream_notify),
                    w);

    g_signal_connect (G_OBJECT (context),
                    "stored-control-added",
                    G_CALLBACK (on_context_stored_control_added),
                    w);
    g_signal_connect (G_OBJECT (context),
                    "stored-control-removed",
                    G_CALLBACK (on_context_stored_control_removed),
                    w);

}

/*
    remove stream
*/
void Widget::on_context_stream_removed (MateMixerContext *context,const gchar *name,Widget *w)
{

//        if (dialog->priv->hw_profile_combo != nullptr) {
//                gboolean show_button;

//                g_object_get (G_OBJECT (dialog->priv->hw_profile_combo),
//                              "show-button", &show_button,
//                              NULL);

//                if (show_button == TRUE)
//                        update_device_test_visibility (dialog);
//        }
    qDebug() << "context stream 移除" << name;
    remove_stream (w, name);
}

/*
    移除stream
*/
void Widget::remove_stream (Widget *w, const gchar *name)
{
//        GtkWidget    *bar;
//        GtkTreeIter   iter;
//        GtkTreeModel *model;
    MateMixerStream *stream = mate_mixer_context_get_stream(w->context,name);
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);
    bool status;
    if (direction == MATE_MIXER_DIRECTION_INPUT) {
        status = w->input_stream_list->removeOne(name);
        qDebug() << "移除input stream list " << status;
    }
    else if (direction == MATE_MIXER_DIRECTION_OUTPUT) {
        status = w->output_stream_list->removeOne(name);
        qDebug() << "移除input stream list " << status;
    }

//        bar = g_hash_table_lookup (dialog->priv->bars, name);

        if (w->app_volume_list != nullptr) {
//                g_debug ("Removing stream %s from bar %s",
//                         name,
//                         gvc_channel_bar_get_name (GVC_CHANNEL_BAR (bar)));

                bar_set_stream (w,  NULL);
        }

        /* Remove from any models */
//        model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->output_treeview));
//        if (find_tree_item_by_name (GTK_TREE_MODEL (model),
//                                    name,
//                                    NAME_COLUMN,
//                                    &iter) == TRUE)
//                gtk_list_store_remove (GTK_LIST_STORE (model), &iter);

//        model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->input_treeview));
//        if (find_tree_item_by_name (GTK_TREE_MODEL (model),
//                                    name,
//                                    NAME_COLUMN,
//                                    &iter) == TRUE)
//                gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
}

/*
    context 添加设备并设置到单选框
*/
void Widget::on_context_device_added (MateMixerContext *context, const gchar *name, Widget *w)
{
    MateMixerDevice *device;
    device = mate_mixer_context_get_device (context, name);
    qDebug() << "748 context 设备添加" << mate_mixer_device_get_label(device) <<name;

    if (G_UNLIKELY (device == nullptr))
            return;
    add_device (w, device);
}

/*
    添加设备
*/
void Widget::add_device (Widget *w, MateMixerDevice *device)
{
//        GtkTreeModel    *model;
//        GtkTreeIter      iter;
//        GIcon           *icon;
    const gchar *name;
    const gchar *label;
    gchar *status;
    const gchar *profile_label = nullptr;
    MateMixerSwitch *profile_switch;

//        model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->hw_treeview));

    name  = mate_mixer_device_get_name (device);
    label = mate_mixer_device_get_label (device);
    w->device_name_list->append(name);
    //添加设备到组合框
    w->ui->inputDeviceCombobox->addItem(label);
    w->ui->outputDeviceCombobox->addItem(label);

}

/*
    移除设备
*/
void Widget::on_context_device_removed (MateMixerContext *context,const gchar *name,Widget *w)
{
    int  count = 0;
    MateMixerDevice *dev = mate_mixer_context_get_device(context,name);
    QString str = mate_mixer_device_get_label(dev);
    do {
        if (name == w->device_name_list->at(count)) {
//            qDebug() << "context设备移除" << name << "移除的设备名为" << w->device_name_list->at(count);
            qDebug() << "device remove";
            w->device_name_list->removeAt(count);
            w->ui->outputDeviceCombobox->removeItem(count);
            w->ui->inputDeviceCombobox->removeItem(count);
            break;
        }
        count++;
        if (count > w->device_name_list->size()) {
            qDebug() << "device error";
            break;
        }
    }while(1);

    w->ui->inputDeviceCombobox->removeItem(1);
    w->ui->outputDeviceCombobox->removeItem(1);
    if (dev == nullptr)
        qDebug() << "device is null";
}

/*
    默认输入流通知
*/
void Widget::on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,Widget *w)
{
    MateMixerStream *stream;

    g_debug ("Default input stream has changed");
    stream = mate_mixer_context_get_default_input_stream (context);

    set_input_stream (w, stream);
}

void Widget::set_input_stream (Widget *w, MateMixerStream *stream)
{
//        GtkTreeModel           *model;
    MateMixerSwitch        *swtch;
    MateMixerStreamControl *control;

//        control = gvc_channel_bar_get_control (GVC_CHANNEL_BAR (dialog->priv->input_bar));
    if (control != nullptr) {
//                /* Disconnect port switch of the previous stream */
//                if (dialog->priv->input_port_combo != nullptr) {
//                        swtch = g_object_get_data (G_OBJECT (dialog->priv->input_port_combo),
//                                                   "switch");
//                        if (swtch != nullptr)
//                                g_signal_handlers_disconnect_by_data (G_OBJECT (swtch),
//                                                                      dialog);
//                }

            /* Disable monitoring of the previous control */
//                g_signal_handlers_disconnect_by_func (G_OBJECT (control),
//                                                      G_CALLBACK (on_stream_control_monitor_value),
//                                                      dialog);

        mate_mixer_stream_control_set_monitor_enabled (control, FALSE);
    }

    bar_set_stream (w, stream);

    if (stream != nullptr) {
        const GList *controls;
//                guint page = gtk_notebook_get_current_page (GTK_NOTEBOOK (dialog->priv->notebook));

        controls = mate_mixer_context_list_stored_controls (w->context);

        /* Move all stored controls to the newly selected default stream */
        while (controls != nullptr) {
            MateMixerStream *parent;

            control = MATE_MIXER_STREAM_CONTROL (controls->data);
            parent  = mate_mixer_stream_control_get_stream (control);

            /* Prefer streamless controls to stay the way they are, forcing them to
             * a particular owning stream would be wrong for eg. event controls */
            if (parent != nullptr && parent != stream) {
                MateMixerDirection direction =
                    mate_mixer_stream_get_direction (parent);

                if (direction == MATE_MIXER_DIRECTION_INPUT)
                    mate_mixer_stream_control_set_stream (control, stream);
            }
            controls = controls->next;
        }

//                if (page == PAGE_INPUT) {
//                        MateMixerStreamControl *control =
//                                gvc_channel_bar_get_control (GVC_CHANNEL_BAR (dialog->priv->input_bar));

//                        if (G_LIKELY (control != nullptr))
//                                mate_mixer_stream_control_set_monitor_enabled (control, TRUE);
//                }

        /* Enable/disable the peak level monitor according to mute state */
        g_signal_connect (G_OBJECT (stream),
                          "notify::mute",
                          G_CALLBACK (on_stream_control_mute_notify),
                          w);
    }

//        model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->input_treeview));
//        update_default_tree_item (dialog, model, stream);

//        update_input_settings (dialog);
}

/*
    control 静音通知
*/
void Widget::on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,Widget *dialog)
{
    /* Stop monitoring the input stream when it gets muted */
    if (mate_mixer_stream_control_get_mute (control) == TRUE)
        mate_mixer_stream_control_set_monitor_enabled (control, FALSE);
    else
        mate_mixer_stream_control_set_monitor_enabled (control, TRUE);
}

/*
    默认输出流通知
*/
void Widget::on_context_default_output_stream_notify (MateMixerContext *context,GParamSpec *pspec,Widget *w)
{
    MateMixerStream *stream;
    qDebug() << "默认的输出stream改变";
    stream = mate_mixer_context_get_default_output_stream (context);
    update_icon_output(w);
    set_output_stream (w, stream);
}

/*
    移除存储control
*/
void Widget::on_context_stored_control_removed (MateMixerContext *context,const gchar *name,Widget *w)
{
//        GtkWidget *bar;

//        bar = g_hash_table_lookup (dialog->priv->bars, name);

    if (w->app_volume_list != nullptr) {
            /* We only use a stored control in the effects bar */
//                if (G_UNLIKELY (bar != dialog->priv->effects_bar)) {
//                        g_warn_if_reached ();
//                        return;
//                }

            bar_set_stream (w, NULL);
    }
}

/*
 * context设置属性
*/
void Widget::context_set_property(Widget *w)//,guint prop_id,const GValue *value,GParamSpec *pspec)
{
//    Widget *self = GVC_MIXER_DIALOG (object);
//    mate_mixer_context_set_backend_type(w->context,MATE_MIXER_BACKEND_PULSEAUDIO);

//            switch (prop_id) {
//            case PROP_CONTEXT:
    set_context(w,w->context);

//                    break;
//            default:
//                    qDebug() << 2;
//                    G_OBJECT_WARN_INVALID_PROPERTY_ID (w, prop_id, pspec);
//                    break;
//            }
}

/*
    输出音量控制
*/
void Widget::output_volume_slider_changed_slot(int volume)
{

//    stream = mate_mixer_context_get_stream(context)
    QString percent = QString::number(volume);
    percent.append("%");
    ui->opVolumePercentLabel->setText(percent);

}

/*
    输入音量控制
*/
void Widget::input_volume_slider_changed_slot(int volume)
{
    QString percent = QString::number(volume);
    percent.append("%");
    ui->ipVolumePercentLabel->setText(percent);
}

/*
    更新输入音量及图标
*/
void Widget::update_icon_input (Widget *w)
{
    MateMixerStream        *stream;
    MateMixerStreamControl *control = nullptr;
    const gchar *app_id;
    gboolean show = FALSE;

    stream = mate_mixer_context_get_default_input_stream (w->context);

    const GList *inputs =mate_mixer_stream_list_controls(stream);
    control = mate_mixer_stream_get_default_control(stream);

    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(control);
    int value = volume *100 /65536.0+0.5;
    w->ui->ipVolumeSlider->setValue(value);
    QString percent = QString::number(value);
    percent.append("%");
    w->ui->ipVolumePercentLabel->setText(percent);
    w->ui->input_icon_btn->setFocusPolicy(Qt::NoFocus);
    w->ui->input_icon_btn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");

    QIcon icon;
    const QSize icon_size = QSize(24,24);
    w->ui->input_icon_btn->setIconSize(icon_size);
    if (value <= 0) {
        w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-mute.png"));
    }
    else if (value > 0 && value <= 33) {
        w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-low.png"));
    }
    else if (value >33 && value <= 66) {

        w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-medium.png"));
    }
    else {
        w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-high.png"));
    }

    while (inputs != nullptr) {
        MateMixerStreamControl *input = MATE_MIXER_STREAM_CONTROL (inputs->data);
        MateMixerStreamControlRole role = mate_mixer_stream_control_get_role (input);
        if (role == MATE_MIXER_STREAM_CONTROL_ROLE_APPLICATION) {
            MateMixerAppInfo *app_info = mate_mixer_stream_control_get_app_info (input);
            app_id = mate_mixer_app_info_get_id (app_info);
            if (app_id == nullptr) {
                /* A recording application which has no
                 * identifier set */
                g_debug ("Found a recording application control %s",
                    mate_mixer_stream_control_get_label (input));

                if G_UNLIKELY (control == nullptr) {
                    /* In the unlikely case when there is no
                     * default input control, use the application
                     * control for the icon */
                    control = input;
                }
                show = TRUE;
                break;
            }

            if (strcmp (app_id, "org.mate.VolumeControl") != 0 &&
                strcmp (app_id, "org.gnome.VolumeControl") != 0 &&
                strcmp (app_id, "org.PulseAudio.pavucontrol") != 0) {
                    g_debug ("Found a recording application %s", app_id);

                    if G_UNLIKELY (control == nullptr)
                            control = input;

                    show = TRUE;
                    break;
            }
        }
        inputs = inputs->next;
    }

        if (show == TRUE)
                g_debug ("Input icon enabled");
        else
                g_debug ("There is no recording application, input icon disabled");

        connect(w->ui->ipVolumeSlider,&QSlider::valueChanged,[=](int value){
            QString percent;
            if (value <= 0) {
                mate_mixer_stream_control_set_mute(control,TRUE);
                mate_mixer_stream_control_set_volume(control,0);
                percent = QString::number(0);
                w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-mute.png"));
            }
            else if (value > 0 && value <= 33) {
                w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-low.png"));
            }
            else if (value >33 && value <= 66) {
                w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-medium.png"));
            }
            else {
                w->ui->input_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/microphone-high.png"));
            }
            percent = QString::number(value);
            mate_mixer_stream_control_set_mute(control,FALSE);
            int volume = value*65536/100;
            gboolean ok = mate_mixer_stream_control_set_volume(control,volume);
            percent.append("%");
            w->ui->ipVolumePercentLabel->setText(percent);
        });
        gvc_stream_status_icon_set_control (w, control);

        if (control != nullptr) {
                g_debug ("Output icon enabled");
                qDebug() << "control is not null";
//                gtk_status_icon_set_visible (GTK_STATUS_ICON (applet->priv->icon_output),
//                                             TRUE);
        }
        else {
                g_debug ("There is no output stream/control, output icon disabled");
                qDebug() << "control is  null";
//                gtk_status_icon_set_visible (GTK_STATUS_ICON (applet->priv->icon_output),
//                                             FALSE);
        }
}

/*
    更新输出音量及图标
*/
void Widget::update_icon_output (Widget *w)
{
    MateMixerStream        *stream;
    MateMixerStreamControl *control = nullptr;

    stream = mate_mixer_context_get_default_output_stream (w->context);
    if (stream != nullptr)
            control = mate_mixer_stream_get_default_control (stream);

    gvc_stream_status_icon_set_control (w, control);
    //初始化滑动条的值
    int volume = mate_mixer_stream_control_get_volume(control);
    int value = volume *100 /65536.0+0.5;
    w->ui->opVolumeSlider->setValue(value);
    QString percent = QString::number(value);
    percent.append("%");
    w->ui->opVolumePercentLabel->setText(percent);
    w->ui->output_icon_btn->setFocusPolicy(Qt::NoFocus);
    w->ui->output_icon_btn->setStyleSheet("QPushButton{background:transparent;border:0px;padding-left:0px;}");
    QIcon icon;
    const QSize icon_size = QSize(24,24);
    w->ui->output_icon_btn->setIconSize(icon_size);
    if (value <= 0) {
        w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-mute.png"));
    }
    else if (value > 0 && value <= 33) {
        w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-low.png"));
    }
    else if (value >33 && value <= 66) {

        w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-medium.png"));
    }
    else {
        w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-high.png"));
    }


    //输出音量控制
    //输出滑动条和音量控制
    connect(w->ui->opVolumeSlider,&QSlider::valueChanged,[=](int value){
        QString percent;
        if (value <= 0) {
            mate_mixer_stream_control_set_mute(control,TRUE);
            mate_mixer_stream_control_set_volume(control,0);
            percent = QString::number(0);
            w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-mute.png"));
        }
        else if (value > 0 && value <= 33) {
            w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-low.png"));
        }
        else if (value >33 && value <= 66) {
            w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-medium.png"));
        }
        else {
            w->ui->output_icon_btn->setIcon(QIcon("/usr/share/ukui-media/img/audio-high.png"));
        }
        percent = QString::number(value);
        mate_mixer_stream_control_set_mute(control,FALSE);
        int volume = value*65536/100;
        gboolean ok = mate_mixer_stream_control_set_volume(control,volume);
        percent.append("%");
        w->ui->opVolumePercentLabel->setText(percent);
    });
    if (control != nullptr) {
            g_debug ("Output icon enabled");
//                gtk_status_icon_set_visible (GTK_STATUS_ICON (applet->priv->icon_output),
//                                             TRUE);
    }
    else {
            g_debug ("There is no output stream/control, output icon disabled");
//                gtk_status_icon_set_visible (GTK_STATUS_ICON (applet->priv->icon_output),
//                                             FALSE);
    }
}

void Widget::gvc_stream_status_icon_set_control (Widget *w,MateMixerStreamControl *control)
{
//        g_return_if_fail (GVC_STREAM_STATUS_ICON (icon));

//        if (icon->priv->control == control)
//                return;

    g_signal_connect ( G_OBJECT (control),
                      "notify::volume",
                      G_CALLBACK (on_stream_control_volume_notify),
                      w);
    g_signal_connect (G_OBJECT (control),
                      "notify::mute",
                      G_CALLBACK (on_stream_control_mute_notify),
                      w);

    MateMixerDirection direction = mate_mixer_stored_control_get_direction((MateMixerStoredControl *)control);
    if (direction == MATE_MIXER_DIRECTION_OUTPUT)
        qDebug() << "output******************";
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
        qDebug() << "input*****************";
    }
//    connect(w->ui->opVolumeSlider,SIGNAL(valueChanged(int)),w,SLOT(output_volume_slider_changed_slot(int)));

        if (control != nullptr)
//                g_object_ref (control);

        if (control != nullptr) {
//                g_signal_handlers_disconnect_by_func (G_OBJECT (control),
//                                                      G_CALLBACK (on_stream_control_volume_notify),
//                                                     w);
//                g_signal_handlers_disconnect_by_func (G_OBJECT (control),
//                                                      G_CALLBACK (on_stream_control_mute_notify),
//                                                      w);

//                g_object_unref (control);
        }

//        icon->priv->control = control;

//        if (icon->priv->control != nullptr) {

                // XXX when no stream set some default icon and "unset" dock
//                update_icon (icon);
//        }

//        gvc_channel_bar_set_control (GVC_CHANNEL_BAR (icon->priv->bar), icon->priv->control);

//        g_object_notify_by_pspec (G_OBJECT (icon), properties[PROP_CONTROL]);
}

/*
    stream control 声音通知
*/
void Widget::on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,Widget *w)
{
    MateMixerStreamControlFlags flags;
    gboolean muted = FALSE;
    gdouble decibel = 0;
    guint volume = 0;
    guint normal = 0;
    QString decscription;

//    qDebug() << "control" << mate_mixer_stream_control_get_name() << volume;
    if (control != nullptr)
        flags = mate_mixer_stream_control_get_flags(control);

    if(flags&MATE_MIXER_STREAM_CONTROL_MUTE_READABLE)
        muted = mate_mixer_stream_control_get_mute(control);

    if (flags&MATE_MIXER_STREAM_CONTROL_VOLUME_READABLE) {
        volume = mate_mixer_stream_control_get_volume(control);
        normal = mate_mixer_stream_control_get_normal_volume(control);
    }

    if (flags&MATE_MIXER_STREAM_CONTROL_HAS_DECIBEL)
        decibel = mate_mixer_stream_control_get_decibel(control);
//        update_icon (icon);
    decscription = mate_mixer_stream_control_get_label(control);

    MateMixerStream *stream = mate_mixer_stream_control_get_stream(control);
    MateMixerDirection direction = mate_mixer_stream_get_direction(stream);

    //设置输出滑动条的值
    int value = volume*100/65536.0 + 0.5;
    if (direction == MATE_MIXER_DIRECTION_OUTPUT) {

        w->ui->opVolumeSlider->setValue(value);
    }
    else if (direction == MATE_MIXER_DIRECTION_INPUT) {
//        qDebug() << "输入" << value;
        w->ui->ipVolumeSlider->setValue(value);
    }

//    qDebug() << "音量值为" << volume << value;



}

/*
    设置平衡属性
*/
void Widget::gvc_balance_bar_set_property (Widget *w,MateMixerStreamControl *control)
{
//        GvcBalanceBar *self = GVC_BALANCE_BAR (object);
    gvc_balance_bar_set_control(w,control);

    gvc_balance_bar_set_balance_type (w,control);

}

/*
    平衡设置control
*/
void Widget::gvc_balance_bar_set_control (Widget *w, MateMixerStreamControl *control)
{
//        if (bar->priv->btype == BALANCE_TYPE_LFE) {
    gdouble minimum;
    gdouble maximum;

    minimum = mate_mixer_stream_control_get_min_volume (control);
    maximum = mate_mixer_stream_control_get_normal_volume (control);

    /* Configure the adjustment for the volume limits of the current
     * stream.
     * Only subwoofer scale uses volume, balance and fade use fixed
     * limits which do not need to be updated as balance type is
     * only set during construction. */
//                gtk_adjustment_configure (GTK_ADJUSTMENT (bar->priv->adjustment),
//                                          gtk_adjustment_get_value (bar->priv->adjustment),
//                                          minimum,
//                                          maximum,
//                                          (maximum - minimum) / 100.0,
//                                          (maximum - minimum) / 10.0,
//                                          0.0);

//                bar->priv->lfe_channel = find_stream_lfe_channel (bar->priv->control);

//                if (G_LIKELY (bar->priv->lfe_channel > -1))
//                        g_debug ("Found LFE channel at position %d", bar->priv->lfe_channel);
//                else
//                        g_warn_if_reached ();

//                bar->priv->lfe_channel = -1;

//平衡设置
//        switch (bar->priv->btype) {
//        case BALANCE_TYPE_RL:
    g_signal_connect (G_OBJECT (control),
                      "notify::balance",
                      G_CALLBACK (on_balance_value_changed),
                      w);
//                break;
//        case BALANCE_TYPE_FR:
//                g_signal_connect (G_OBJECT (bar->priv->control),
//                                  "notify::fade",
//                                  G_CALLBACK (on_balance_value_changed),
//                                  bar);
//                break;
//        case BALANCE_TYPE_LFE:
//                g_signal_connect (G_OBJECT (bar->priv->control),
//                                  "notify::volume",
//                                  G_CALLBACK (on_balance_value_changed),
//                                  bar);
//                break;
//        }

//        update_balance_value (bar);
//        update_scale_marks (bar);

//        g_object_notify_by_pspec (G_OBJECT (bar), properties[PROP_CONTROL]);
}

/*
    设置平衡类型
*/
void Widget::gvc_balance_bar_set_balance_type (Widget *w, MateMixerStreamControl *control)
{
//        GtkWidget     *frame;
//        GtkAdjustment *adjustment;

        /* Create adjustment with limits for balance and fade types because
         * some limits must be provided.
         * If subwoofer type is used instead, the limits will be changed when
         * stream is set. */
//        adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, -1.0, 1.0, 0.05, 0.5, 0.0));

//        bar->priv->btype = btype;
//        bar->priv->adjustment = GTK_ADJUSTMENT (g_object_ref_sink (adjustment));

//        g_signal_connect (G_OBJECT (adjustment),
//                          "value-changed",
//                          G_CALLBACK (on_adjustment_value_changed),
//                          bar);

//        switch (btype) {
//        case BALANCE_TYPE_RL:
//                bar->priv->label = gtk_label_new_with_mnemonic (_("_Balance:"));
//                break;
//        case BALANCE_TYPE_FR:
//                bar->priv->label = gtk_label_new_with_mnemonic (_("_Fade:"));
//                break;
//        case BALANCE_TYPE_LFE:
//                bar->priv->label = gtk_label_new_with_mnemonic (_("_Subwoofer:"));
//                break;
//        }

//        gtk_label_set_xalign (GTK_LABEL (bar->priv->label), 0.0);
//        gtk_label_set_yalign (GTK_LABEL (bar->priv->label), 0.0);

//        /* Frame */
//        frame = gtk_frame_new (NULL);
//        gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_NONE);
//        gtk_box_pack_start (GTK_BOX (bar), frame, TRUE, TRUE, 0);

//        /* Box with scale */
//        create_scale_box (bar);
//        gtk_container_add (GTK_CONTAINER (frame), bar->priv->scale_box);

//        gtk_label_set_mnemonic_widget (GTK_LABEL (bar->priv->label),
//                                       bar->priv->scale);

//        gtk_widget_set_direction (bar->priv->scale, GTK_TEXT_DIR_LTR);
//        gtk_widget_show_all (frame);

//        g_object_notify_by_pspec (G_OBJECT (bar), properties[PROP_BALANCE_TYPE]);
}

/*
    平衡值改变
*/
void Widget::on_balance_value_changed (MateMixerStreamControl *control,GParamSpec *pspec,Widget *w)
{
//        update_balance_value (w);
    gdouble value = mate_mixer_stream_control_get_balance(control);
//    qDebug() << "平衡值改变" << value*100;
    w->ui->opBalanceSlider->setValue(value*100);
}

/*
    更新输出设置
*/
void Widget::update_output_settings (Widget *w,MateMixerStreamControl *control)
{
    MateMixerStreamControlFlags flags;
    flags = mate_mixer_stream_control_get_flags(control);

    if (flags & MATE_MIXER_STREAM_CONTROL_CAN_BALANCE) {
        gvc_balance_bar_set_property(w,control);
    }

    connect(w->ui->opBalanceSlider,&QSlider::valueChanged,[=](int volume){
        gdouble value = volume/100.0;

        mate_mixer_stream_control_set_balance(control,value);

    });
}

void Widget::on_key_changed (GSettings *settings,gchar *key,Widget *w)
{
    if (!strcmp (key, EVENT_SOUNDS_KEY) ||
        !strcmp (key, SOUND_THEME_KEY) ||
        !strcmp (key, INPUT_SOUNDS_KEY)) {
        update_theme (w);

    }

}

/*
    更新主题
*/
void Widget::update_theme (Widget *w)
{
    char        *theme_name;
    gboolean     events_enabled;
    gboolean     feedback_enabled;

    feedback_enabled = g_settings_get_boolean (w->sound_settings, INPUT_SOUNDS_KEY);
//    set_input_feedback_enabled (w, feedback_enabled);

    events_enabled = g_settings_get_boolean (w->sound_settings, EVENT_SOUNDS_KEY);
    if (events_enabled) {
        theme_name = g_settings_get_string (w->sound_settings, SOUND_THEME_KEY);
    } else {
        theme_name = g_strdup (NO_SOUNDS_THEME_NAME);
    }

    qDebug() << "主题名" << theme_name;
//    gtk_widget_set_sensitive (chooser->priv->selection_box, events_enabled);
//    gtk_widget_set_sensitive (chooser->priv->click_feedback_button, events_enabled);

    //设置combox的主题
    set_combox_for_theme_name (w, theme_name);
    update_alerts_from_theme_name (w, theme_name);

}

/*
    设置主题名到combox
*/
void Widget::setup_theme_selector (Widget *w)
{
    GHashTable           *hash;
    const char * const   *data_dirs;
    const char           *data_dir;
    char                 *dir;
    guint                 i;

    /* Add the theme names and their display name to a hash table,
     * makes it easy to avoid duplicate themes */
    hash = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

    data_dirs = g_get_system_data_dirs ();
    for (i = 0; data_dirs[i] != NULL; i++) {
            dir = g_build_filename (data_dirs[i], "sounds", NULL);
            sound_theme_in_dir (w,hash, dir);
//            g_free (dir);
    }

    data_dir = g_get_user_data_dir ();
    dir = g_build_filename (data_dir, "sounds", NULL);
    sound_theme_in_dir (w,hash, dir);
//    g_free (dir);

    /* If there isn't at least one theme, make everything
     * insensitive, LAME! */
    if (g_hash_table_size (hash) == 0) {
//            gtk_widget_set_sensitive (GTK_WIDGET (chooser), FALSE);
            g_warning ("Bad setup, install the freedesktop sound theme");
            g_hash_table_destroy (hash);
            return;
    }

    /* Add the themes to a combobox */
    g_hash_table_destroy (hash);

    /* Set the display */


//        g_signal_connect (G_OBJECT (chooser->priv->combo_box),
//                          "changed",
//                          G_CALLBACK (on_combobox_changed),
//                          chooser);
}

/*
    主题名所在目录
*/
void Widget::sound_theme_in_dir (Widget *w,GHashTable *hash,const char *dir)
{
    GDir *d;
    const char *name;

    d = g_dir_open (dir, 0, NULL);
    if (d == NULL) {
        return;
    }

    while ((name = g_dir_read_name (d)) != NULL) {
        char *dirname, *index, *indexname;
        /* Look for directories */
        dirname = g_build_filename (dir, name, NULL);
        if (g_file_test (dirname, G_FILE_TEST_IS_DIR) == FALSE) {
//                g_free (dirname);
                continue;
        }

        /* Look for index files */
        index = g_build_filename (dirname, "index.theme", NULL);
//        g_free (dirname);

        /* Check the name of the theme in the index.theme file */
        indexname = load_index_theme_name (index, NULL);
//        g_free (index);
        if (indexname == NULL) {
            continue;
        }
        //设置主题到combox中
        w->theme_display_name_list->append(indexname);
        w->theme_name_list->append(name);
        qDebug() << "-=-=-=-=-0" << indexname << name;
        w->ui->soundThemeCombobox->addItem(indexname);
//        g_hash_table_insert (hash, g_strdup (name), indexname);
    }

    g_dir_close (d);
}

/*
    加载下标的主题名
*/
char *Widget::load_index_theme_name (const char *index,char **parent)
{
    GKeyFile *file;
    char *indexname = NULL;
    gboolean hidden;

    file = g_key_file_new ();
    if (g_key_file_load_from_file (file, index, G_KEY_FILE_KEEP_TRANSLATIONS, NULL) == FALSE) {
        g_key_file_free (file);
        return NULL;
    }
    /* Don't add hidden themes to the list */
    hidden = g_key_file_get_boolean (file, "Sound Theme", "Hidden", NULL);
    if (!hidden) {
        indexname = g_key_file_get_locale_string (file,
                                                  "Sound Theme",
                                                  "Name",
                                                  NULL,
                                                  NULL);

            /* Save the parent theme, if there's one */
        if (parent != NULL) {
            *parent = g_key_file_get_string (file,
                                             "Sound Theme",
                                             "Inherits",
                                             NULL);
        }
    }

    g_key_file_free (file);
    return indexname;
}

/*
    设置combox的主题名
*/
void Widget::set_combox_for_theme_name (Widget *w,const char *name)
{

    gboolean      found;
    static int count = 0;
    /* If the name is empty, use "freedesktop" */
    if (name == NULL || *name == '\0') {
        name = "freedesktop";
    }

//    model = gtk_combo_box_get_model (GTK_COMBO_BOX (chooser->priv->combo_box));

//    if (gtk_tree_model_get_iter_first (model, &iter) == FALSE) {
//            return;
//    }
    QString value;
    while(!found) {
        value = w->theme_name_list->at(count);

//            gtk_tree_model_get (model, &iter, THEME_IDENTIFIER_COL, &value, -1);
        found = (value != "" && value == name);
//            g_free (value);
        count++;
//        qDebug() << "Found" << found << "name"<< name << "主题名" << value << count << "size" <<  w->theme_name_list->size();
        if( count >= w->theme_name_list->size() || found) {
            count = 0;

            break;
        }
    }
    /* When we can't find the theme we need to set, try to set the default
     * one "freedesktop" */
    if (found) {
        qDebug() << "设置combox值" << value;
        w->ui->soundThemeCombobox->setCurrentText(value);
//            gtk_combo_box_set_active_iter (GTK_COMBO_BOX (chooser->priv->combo_box), &iter);
    } else if (strcmp (name, "freedesktop") != 0) {//设置为默认的主题
        g_debug ("not found, falling back to fdo");
        set_combox_for_theme_name (w, "freedesktop");
    }
}

/*
    更新报警音
*/
void Widget::update_alerts_from_theme_name (Widget *w,const gchar *name)
{
    if (strcmp (name, CUSTOM_THEME_NAME) != 0) {
            /* reset alert to default */
        update_alert (w, DEFAULT_ALERT_ID);
    } else {
        int   sound_type;
        char *linkname;

        linkname = NULL;
        sound_type = get_file_type ("bell-terminal", &linkname);
        g_debug ("Found link: %s", linkname);
        if (sound_type == SOUND_TYPE_CUSTOM) {
            update_alert (w, linkname);
        }
    }
}

/*
    更新报警声音
*/
void Widget::update_alert (Widget *w,const char *alert_id)
{
    QString theme;
    char *parent;
    gboolean is_custom;
    gboolean is_default;
    gboolean add_custom;
    gboolean remove_custom;

    /* Get the current theme's name, and set the parent */

    theme = w->ui->soundThemeCombobox->currentText();
    is_custom = (theme == CUSTOM_THEME_NAME) ;
    is_default = strcmp (alert_id, DEFAULT_ALERT_ID) == 0;

    /* So a few possibilities:
     * 1. Named theme, default alert selected: noop
     * 2. Named theme, alternate alert selected: create new custom with sound
     * 3. Custom theme, default alert selected: remove sound and possibly custom
     * 4. Custom theme, alternate alert selected: update custom sound
     */
    add_custom = FALSE;
    remove_custom = FALSE;
    if (! is_custom && is_default) {
            /* remove custom just in case */
        remove_custom = TRUE;
    } else if (! is_custom && ! is_default) {
//                create_custom_theme (parent);
//                save_alert_sounds (chooser, alert_id);
        add_custom = TRUE;
    } else if (is_custom && is_default) {
        //                save_alert_sounds (chooser, alert_id);
            /* after removing files check if it is empty */
//                if (custom_theme_dir_is_empty ()) {
//                        remove_custom = TRUE;
//                }
    } else if (is_custom && ! is_default) {
        //                save_alert_sounds (chooser, alert_id);
    }

    if (add_custom) {
//                gtk_list_store_insert_with_values (GTK_LIST_STORE (theme_model),
//                                                   NULL,
//                                                   G_MAXINT,
//                                                   THEME_DISPLAY_COL, _("Custom"),
//                                                   THEME_IDENTIFIER_COL, CUSTOM_THEME_NAME,
//                                                   THEME_PARENT_ID_COL, theme,
//                                                   -1);
        set_combox_for_theme_name (w, CUSTOM_THEME_NAME);
    } else if (remove_custom) {
//                gtk_tree_model_get_iter_first (theme_model, &iter);
//                do {
//                        char *this_parent;

//                        gtk_tree_model_get (theme_model, &iter,
//                                            THEME_PARENT_ID_COL, &this_parent,
//                                            -1);
//                        if (this_parent != NULL && strcmp (this_parent, CUSTOM_THEME_NAME) != 0) {
//                                g_free (this_parent);
//                                gtk_list_store_remove (GTK_LIST_STORE (theme_model), &iter);
//                                break;
//                        }
//                        g_free (this_parent);
//                } while (gtk_tree_model_iter_next (theme_model, &iter));

//                delete_custom_theme_dir ();

        set_combox_for_theme_name (w, parent);
    }

//    update_alert_model (w, alert_id);

//    g_free (parent);
}

/*
    获取声音文件类型
*/
int Widget::get_file_type (const char *sound_name,char **linked_name)
{
    char *name, *filename;
    *linked_name = NULL;

    name = g_strdup_printf ("%s.disabled", sound_name);
    filename = custom_theme_dir_path (name);
//        g_free (name);

    if (g_file_test (filename, G_FILE_TEST_IS_REGULAR) != FALSE) {
//                g_free (filename);
        return SOUND_TYPE_OFF;
    }
//        g_free (filename);

    /* We only check for .ogg files because those are the
     * only ones we create */
    name = g_strdup_printf ("%s.ogg", sound_name);
    filename = custom_theme_dir_path (name);
    g_free (name);

    if (g_file_test (filename, G_FILE_TEST_IS_SYMLINK) != FALSE) {
        *linked_name = g_file_read_link (filename, NULL);
        g_free (filename);
        return SOUND_TYPE_CUSTOM;
    }
    g_free (filename);

    return SOUND_TYPE_BUILTIN;
}

/*
    自定义主题路径
*/
char *Widget::custom_theme_dir_path (const char *child)
{
    static char *dir = NULL;
    const char *data_dir;

    if (dir == NULL) {
        data_dir = g_get_user_data_dir ();
        dir = g_build_filename (data_dir, "sounds", CUSTOM_THEME_NAME, NULL);
    }
    if (child == NULL)
        return g_strdup (dir);

    return g_build_filename (dir, child, NULL);
}

/*
    获取报警声音文件的路径
*/
void Widget::populate_model_from_dir (Widget *w,const char *dirname)//从目录查找报警声音文件
{
    GDir       *d;
    const char *name;

    d = g_dir_open (dirname, 0, NULL);
    if (d == NULL) {
        return;
    }
    while ((name = g_dir_read_name (d)) != NULL) {
        char *path;

        if (! g_str_has_suffix (name, ".xml")) {
            continue;
        }

        path = g_build_filename (dirname, name, NULL);
        populate_model_from_file (w, path);
        g_free (path);
    }

    g_dir_close (d);
}

/*
    获取报警声音文件
*/
void Widget::populate_model_from_file (Widget *w,const char *filename)
{
    xmlDocPtr  doc;
    xmlNodePtr root;
    xmlNodePtr child;
    gboolean   exists;

    exists = g_file_test (filename, G_FILE_TEST_EXISTS);
    if (! exists) {
        return;
    }

    doc = xmlParseFile (filename);
    if (doc == NULL) {
        return;
    }

    root = xmlDocGetRootElement (doc);

    for (child = root->children; child; child = child->next) {
        if (xmlNodeIsText (child)) {
                continue;
        }
        if (xmlStrcmp (child->name, GVC_SOUND_SOUND) != 0) {
                continue;
        }

        populate_model_from_node (w, child);
    }

    xmlFreeDoc (doc);
}

/*
    从节点查找声音文件并加载到组合框中
*/
void Widget::populate_model_from_node (Widget *w,xmlNodePtr node)
{
    xmlNodePtr child;
    xmlChar   *filename;
    xmlChar   *name;

    filename = NULL;
    name = xml_get_and_trim_names (node);
    for (child = node->children; child; child = child->next) {
        if (xmlNodeIsText (child)) {
            continue;
        }

        if (xmlStrcmp (child->name, GVC_SOUND_FILENAME) == 0) {
            filename = xmlNodeGetContent (child);
        } else if (xmlStrcmp (child->name, GVC_SOUND_NAME) == 0) {
                /* EH? should have been trimmed */
        }
    }

    //将找到的声音文件名设置到combox中
    if (filename != NULL && name != NULL) {
//        char *sound_name = name;
        w->soundlist->append((const char *)filename);
        w->ui->shutdownCombobox->addItem((char *)name);
        w->ui->lagoutCombobox->addItem((char *)name);

    }

    xmlFree (filename);
    xmlFree (name);
}

/* Adapted from yelp-toc-pager.c */
xmlChar *Widget::xml_get_and_trim_names (xmlNodePtr node)
{
    xmlNodePtr cur;
    xmlChar *keep_lang = NULL;
    xmlChar *value;
    int j, keep_pri = INT_MAX;

    const gchar * const * langs = g_get_language_names ();

    value = NULL;

    for (cur = node->children; cur; cur = cur->next) {
        if (! xmlStrcmp (cur->name, GVC_SOUND_NAME)) {
            xmlChar *cur_lang = NULL;
            int cur_pri = INT_MAX;

            cur_lang = xmlNodeGetLang (cur);

            if (cur_lang) {
                for (j = 0; langs[j]; j++) {
                    if (g_str_equal (cur_lang, langs[j])) {
                        cur_pri = j;
                        break;
                    }
                }
            } else {
                cur_pri = INT_MAX - 1;
            }

            if (cur_pri <= keep_pri) {
                if (keep_lang)
                    xmlFree (keep_lang);
                if (value)
                    xmlFree (value);

                value = xmlNodeGetContent (cur);

                keep_lang = cur_lang;
                keep_pri = cur_pri;
            } else {
                if (cur_lang)
                    xmlFree (cur_lang);
            }
        }
    }

    /* Delete all GVC_SOUND_NAME nodes */
    cur = node->children;
    while (cur) {
            xmlNodePtr p_this = cur;
            cur = cur->next;
            if (! xmlStrcmp (p_this->name, GVC_SOUND_NAME)) {
                xmlUnlinkNode (p_this);
                xmlFreeNode (p_this);
            }
    }

    return value;
}

/*
 * 播放报警声音
*/
void Widget::play_alret_sound_from_path (QString path)
{
   QMediaPlayer *player = new QMediaPlayer;
//   connect(w->player, SIGNAL(positionChanged(qint64)), w, SLOT(positionChanged(qint64)));
   player->setMedia(QUrl::fromLocalFile(path));
//   w->player->setVolume(30);
   player->play();
}

/*
    点击combox播放声音
*/
void Widget::combox_index_changed_slot(int index)
{
    QString sound_name = soundlist->at(index);
    qDebug() << sound_name;
    play_alret_sound_from_path(sound_name);
}

/*
    点击声音主题实现主题切换
*/
void Widget::theme_combox_index_changed_slot(int index)
{
    qDebug() << "声音主题改变" << theme_display_name_list->at(index);
    //设置系统主题
}

/*
    点击输出设备combox切换设备
*/
void Widget::output_device_combox_index_changed_slot(int index)
{
    qDebug() << "输出组合框index改变" << index  << output_stream_list->at(0);
    MateMixerBackendFlags flags;
    QString name = output_stream_list->at(index);
    //QString转换为const char *
    const char *device_name = name.toLocal8Bit();

    MateMixerStream *stream = mate_mixer_context_get_stream(context,device_name);

    qDebug() << "输出设备切换" << device_name << name << mate_mixer_stream_get_name(stream);
    flags = mate_mixer_context_get_backend_flags (context);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_OUTPUT_STREAM) {
       qDebug() << "default output";
       mate_mixer_context_set_default_output_stream (context, stream);
    }
    else
        set_output_stream (this, stream);
}

/*
    点击输出设备combox切换
*/
void Widget::input_device_combox_index_changed_slot(int index)
{
    MateMixerBackendFlags flags;
    QString name = input_stream_list->at(index);
    //QString转换为const char *
    const char *device_name = name.toLocal8Bit();

    MateMixerStream *stream = mate_mixer_context_get_stream(context,device_name);

    qDebug() << "输入设备切换" << device_name << name << mate_mixer_stream_get_name(stream);
    flags = mate_mixer_context_get_backend_flags (context);

    if (flags & MATE_MIXER_BACKEND_CAN_SET_DEFAULT_INPUT_STREAM) {
        qDebug() << "default input";
        mate_mixer_context_set_default_input_stream (context, stream);
    }
    else
        set_input_stream (this, stream);
}

void Widget::set_output_stream (Widget *w, MateMixerStream *stream)
{
        MateMixerSwitch        *swtch;
        MateMixerStreamControl *control;
        qDebug() << "set output stream";
//        control = gvc_channel_bar_get_control (GVC_CHANNEL_BAR (dialog->priv->output_bar));
//        if (control != NULL) {
//                /* Disconnect port switch of the previous stream */
//                if (dialog->priv->output_port_combo != NULL) {
//                        swtch = g_object_get_data (G_OBJECT (dialog->priv->output_port_combo),
//                                                   "switch");
//                        if (swtch != NULL)
//                                g_signal_handlers_disconnect_by_data (G_OBJECT (swtch),
//                                                                      dialog);
//                }
//        }

        bar_set_stream (w,stream);

        if (stream != NULL) {
                const GList *controls;

                controls = mate_mixer_context_list_stored_controls (w->context);

                /* Move all stored controls to the newly selected default stream */
                while (controls != NULL) {
                        MateMixerStream        *parent;
                        MateMixerStreamControl *control;

                        control = MATE_MIXER_STREAM_CONTROL (controls->data);
                        parent  = mate_mixer_stream_control_get_stream (control);

                        /* Prefer streamless controls to stay the way they are, forcing them to
                         * a particular owning stream would be wrong for eg. event controls */
                        if (parent != NULL && parent != stream) {
                                MateMixerDirection direction =
                                        mate_mixer_stream_get_direction (parent);

                                if (direction == MATE_MIXER_DIRECTION_OUTPUT)
                                        mate_mixer_stream_control_set_stream (control, stream);
                        }
                        controls = controls->next;
                }
        }

//        model = gtk_tree_view_get_model (GTK_TREE_VIEW (dialog->priv->output_treeview));
        update_output_stream_list (w, stream);
        update_output_settings(w,control);
}

/*
    更新输出stream 列表
*/
void Widget::update_output_stream_list(Widget *w,MateMixerStream *stream)
{
    const gchar *name = NULL;
    if (stream != nullptr) {
        name = mate_mixer_stream_get_name(stream);
        w->output_stream_list->append(name);
        qDebug() << "更新输出stream名为" << name;
    }
}

/*
    bar设置stream
*/
void Widget::bar_set_stream (Widget  *w,MateMixerStream *stream)
{
        MateMixerStreamControl *control = NULL;

        if (stream != NULL)
                control = mate_mixer_stream_get_default_control (stream);

        bar_set_stream_control (w, control);
}

void Widget::bar_set_stream_control (Widget *w,MateMixerStreamControl *control)
{
        const gchar *name;
        MateMixerStreamControl *previous;

//        previous = gvc_channel_bar_get_control (GVC_CHANNEL_BAR (bar));
//        if (previous == control)
//                return;

//        if (previous != NULL) {
//                name = mate_mixer_stream_control_get_name (previous);

//                g_debug ("Removing stream control %s from bar %s",
//                         name,
//                         gvc_channel_bar_get_name (GVC_CHANNEL_BAR (bar)));

//                g_signal_handlers_disconnect_by_data (G_OBJECT (previous), dialog);

//                /* This may not do anything because we no longer have the information
//                 * about the owning stream, in case it was an input stream, make
//                 * sure to disconnected from the peak level monitor */
//                mate_mixer_stream_control_set_monitor_enabled (previous, FALSE);

//                g_hash_table_remove (dialog->priv->bars, name);
//        }

//        gvc_channel_bar_set_control (GVC_CHANNEL_BAR (bar), control);

        if (control != NULL) {
                name = mate_mixer_stream_control_get_name (control);
                qDebug() << "********control" << name;
//                g_debug ("Setting stream control %s for bar %s",
//                         name,
//                         gvc_channel_bar_get_name (GVC_CHANNEL_BAR (bar)));

//                g_hash_table_insert (dialog->priv->bars,
//                                     (gpointer) name,
//                                     bar);
//                 printf("bar set stream control name %s\n",name);
//                gtk_widget_set_sensitive (GTK_WIDGET (bar), TRUE);
        } else
            qDebug() << "set true";
//                gtk_widget_set_sensitive (GTK_WIDGET (bar), TRUE);
}


AudioSlider::AudioSlider(QWidget *parent)
{
    Q_UNUSED(parent);
}

AudioSlider::~AudioSlider()
{

}

Widget::~Widget()
{
    delete ui;
}
