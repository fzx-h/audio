#ifndef WIDGET_H
#define WIDGET_H

#include <libmatemixer/matemixer.h>
#include <QStandardItemModel>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QWidget>
#include <QStringList>
#include <QMediaPlayer>
#include <QEvent>
#include <QMouseEvent>
#include <QCoreApplication>
extern "C" {
#include <gio/gio.h>
#include <libxml/tree.h>
}
#define KEY_SOUNDS_SCHEMA "org.mate.sound"
#define EVENT_SOUNDS_KEY "event-sounds"
#define INPUT_SOUNDS_KEY "input-feedback-sounds"
#define SOUND_THEME_KEY "theme-name"

#define DEFAULT_ALERT_ID "__default"
#define CUSTOM_THEME_NAME "__custom"
#define NO_SOUNDS_THEME_NAME "__no_sounds"
namespace Ui {
class Widget;
}

class AudioSlider : public QSlider
{
    Q_OBJECT
public:
    AudioSlider(QWidget *parent = nullptr);
    ~AudioSlider();

protected:
    void mousePressEvent(QMouseEvent *ev)
    {
        //注意应先调用父类的鼠标点击处理事件，这样可以不影响拖动的情况
        QSlider::mousePressEvent(ev);
        //获取鼠标的位置，这里并不能直接从ev中取值（因为如果是拖动的话，鼠标开始点击的位置没有意义了）
        double pos = ev->pos().x() / (double)width();
        setValue(pos *(maximum() - minimum()) + minimum());
        //向父窗口发送自定义事件event type，这样就可以在父窗口中捕获这个事件进行处理
        QEvent evEvent(static_cast<QEvent::Type>(QEvent::User + 1));
        QCoreApplication::sendEvent(parentWidget(), &evEvent);
    }
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    static void list_device(Widget *w,MateMixerContext *context);

    static void gvc_stream_status_icon_set_control (Widget *w,MateMixerStreamControl *control);
    static void context_set_property(Widget *object);//guint prop_id,const GValue *value,GParamSpec *pspec);
    static void on_context_state_notify (MateMixerContext *context,GParamSpec *pspec,Widget	*w);

    static void on_context_stream_added (MateMixerContext *context,const gchar *name,Widget  *w);
    static void on_context_stream_removed (MateMixerContext *context,const gchar *name,Widget *w);
    static void remove_stream (Widget *w, const gchar *name);

    static void add_stream (Widget *w, MateMixerStream *stream,MateMixerContext *context);
    static void add_application_control (Widget *w, MateMixerStreamControl *control);
    static void on_stream_control_added (MateMixerStream *stream,const gchar *name,Widget  *w);
    static void on_stream_control_removed (MateMixerStream *stream,const gchar *name,Widget *w);
    static void remove_application_control (Widget *w,const gchar *name);
    static void add_app_to_tableview(Widget *w,int appnum,QStandardItemModel *standItemModel,const gchar *app_name,QString app_icon_name,MateMixerStreamControl *control);
    static void on_context_stored_control_added (MateMixerContext *context,const gchar      *name,Widget *w);
    static void update_app_volume (MateMixerStreamControl *control, GParamSpec *pspec ,Widget *w);

    static void on_context_device_added (MateMixerContext *context, const gchar *name, Widget *w);
    static void add_device (Widget *w, MateMixerDevice *device);

    static void on_context_device_removed (MateMixerContext *context,const gchar *name,Widget *w);

    static void on_context_default_input_stream_notify (MateMixerContext *context,GParamSpec *pspec,Widget *w);
    static void set_input_stream (Widget *w, MateMixerStream *stream);
    static void on_stream_control_mute_notify (MateMixerStreamControl *control,GParamSpec *pspec,Widget *dialog);

    static void on_context_default_output_stream_notify (MateMixerContext *context,GParamSpec *pspec,Widget *w);

    static void on_context_stored_control_removed (MateMixerContext *context,const gchar *name,Widget *w);
    static void set_context(Widget *w,MateMixerContext *context);

    static void update_icon_input (Widget *w);
    static void update_icon_output (Widget *w);
    static void on_stream_control_volume_notify (MateMixerStreamControl *control,GParamSpec *pspec,Widget *w);

    //平衡
    static void gvc_balance_bar_set_property (Widget *w,MateMixerStreamControl *control);
    static void gvc_balance_bar_set_control (Widget *w, MateMixerStreamControl *control);
    static void gvc_balance_bar_set_balance_type (Widget *w, MateMixerStreamControl *control);

    static void on_balance_value_changed (MateMixerStreamControl *control,GParamSpec *pspec,Widget *w);
    static void update_output_settings (Widget *w,MateMixerStreamControl *control);

    static void on_key_changed (GSettings *settings,gchar *key,Widget *w);
    static void update_theme (Widget *w);

    static void setup_theme_selector (Widget *w);
    static void sound_theme_in_dir (Widget *w,GHashTable *hash,const char *dir);
    static char *load_index_theme_name (const char *index,char **parent);

    static void set_combox_for_theme_name (Widget *w,const char *name);
    static void update_alerts_from_theme_name (Widget *w,const gchar *name);
    static void update_alert (Widget *w,const char *alert_id);
    static int get_file_type (const char *sound_name,char **linked_name);
    static char *custom_theme_dir_path (const char *child);

    static void populate_model_from_dir (Widget *w,const char *dirname);
    static void populate_model_from_file (Widget *w,const char *filename);
    static void populate_model_from_node (Widget *w,xmlNodePtr node);
    static xmlChar *xml_get_and_trim_names (xmlNodePtr node);

    static void play_alret_sound_from_path (QString path);
    static void set_output_stream (Widget *w, MateMixerStream *stream);
    static void update_output_stream_list(Widget *w,MateMixerStream *stream);

    static void bar_set_stream (Widget *w,MateMixerStream *stream);
    static void bar_set_stream_control (Widget *w,MateMixerStreamControl *control);
Q_SIGNALS:
    void app_volume_changed(bool is_mute,int volume,const gchar *app_name);

private Q_SLOTS:
    void app_slider_changed_slot(int volume);
    void app_volume_changed_slot(bool is_mute,int volume,const gchar *app_name);
    void output_volume_slider_changed_slot(int volume);
    void input_volume_slider_changed_slot(int volume);
    void combox_index_changed_slot(int index);
    void theme_combox_index_changed_slot(int index);
    void output_device_combox_index_changed_slot(int index);
    void input_device_combox_index_changed_slot(int index);

private:
    MateMixerContext *context;
    MateMixerStream *stream;
    MateMixerStream *control;
    QStandardItemModel *standItemModel ;

    QLabel *appLabel;
    QPushButton *appIconBtn;
    QLabel *appVolumeLabel;
    QSlider *appSlider;

    QStringList *soundlist;
    QStringList *theme_display_name_list;
    QStringList *theme_name_list;
    QStringList *device_name_list;
    QStringList *device_display_name_list;
    QStringList *output_stream_list;
    QStringList *input_stream_list;
    QStringList *app_volume_list;
    QStringList *stream_control_list;

    GSettings *sound_settings;
    QLabel *app_display_label;
    Ui::Widget *ui;
};



#endif // WIDGET_H
