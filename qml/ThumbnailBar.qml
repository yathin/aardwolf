import QtQuick 1.1

Rectangle {
    id: thumbnailbar
    color: "#00000000"

    property real cheight: 63

    property int count: 0
    property real item_padding: 25

    property real itemHeight: (linear)? itemSml :itemBig
    property real itemWidth:  (linear)? itemSml :itemBig

    property real itemBig: main.preview_size
    property real itemSml: main.small_preview_size

    property int index_padding: 0

    property real previewWidth: width + anchors.leftMargin + anchors.rightMargin
    property real previewHeight: height + anchors.leftMargin + anchors.rightMargin
    property real previewX: -anchors.leftMargin
    property real previewY: -anchors.leftMargin

    property bool fancy: false
    property bool animations_on: false
    property bool smooth_blocker: false

    property int on_preview : -1
    property int last_previewed: -1

    property bool linear: (on_preview != -1)
    property real list_width: (linear && virtual_width>width)?   virtual_width: width;
    property real list_height:(linear || virtual_height<height)? height : virtual_height;
    property real list_x: 0
    property real list_y: 0

    property real virtual_height: (itemHeight+item_padding)*Math.floor((count+index_padding+item_padding-1) / items_per_line) + (itemHeight+item_padding)
    property real virtual_width:  (itemWidth+item_padding)*count

    property int items_per_line: Math.floor(thumbnailbar.list_width/(thumbnailbar.itemWidth+thumbnailbar.item_padding))
    property int rows_number: Math.floor(count / items_per_line) +1
    property int last_line_items: count - items_per_line*(rows_number - 1)

    property int highlighted_row: -1
    property int highlighted_column: -1
    property int highlighted_index: (highlighted_row > -1 && highlighted_column > -1)? highlighted_row*items_per_line + highlighted_column : -1

    signal preview(int index)
    signal select(int index, int modifier)
    signal openDir(string path)
    signal rotateLeft()
    signal rotateRight()
    signal undo()
    signal loadMore(int index)

    onOn_previewChanged: {
        if(on_preview === -1) {
            preview(-1)
        }

        startSmoothBlocker()
    }

    Timer {
        id: anim_disabler_timer
        interval: 573
        running: false
        repeat: false
        onTriggered: animations_on
    }

    Timer {
        id: smooth_blocker_timer
        interval: 837
        running: false
        repeat: false
        onTriggered: {
            smooth_blocker = false
        }
    }

    function startSmoothBlocker() {
        smooth_blocker = true
        smooth_blocker_timer.stop()
        smooth_blocker_timer.start()
    }

    function load(index){
        loadMore(index)
    }

    function add(){
        var component = Qt.createComponent("ThumbnailBarItem.qml");
        var item = component.createObject(thumbnailbar);
        return item
    }

    function next(){
        thumbnailbar.last_previewed = thumbnailbar.on_preview;

        if(thumbnailbar.on_preview == (thumbnailbar.count-1))
        {
            // TODO: emit signal to change page automatically
            return;
        }

        thumbnailbar.on_preview++
        thumbnailbar.fancy = false
        thumbnailbar.preview(thumbnailbar.on_preview)
        thumbnailbar.fancy = true
    }

    function previous(){
        if(thumbnailbar.on_preview == -1)
            return

        if (thumbnailbar.on_preview == 0)
        {
            // TODO: emit signal to change page automatically
            return;
        }

        thumbnailbar.last_previewed = thumbnailbar.on_preview;
        thumbnailbar.fancy = false
        thumbnailbar.on_preview--
        thumbnailbar.preview(thumbnailbar.on_preview)
        thumbnailbar.fancy = true
    }

    function unshow() {
        thumbnailbar.last_previewed = thumbnailbar.on_preview;
        show(-1)
    }

    function show(index){
        thumbnailbar.last_previewed = thumbnailbar.on_preview;
        thumbnailbar.fancy = false
        thumbnailbar.on_preview = index
        thumbnailbar.preview(index)
        thumbnailbar.fancy = true
    }
}
