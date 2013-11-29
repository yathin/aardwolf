import QtQuick 1.1

Item {
    id: thumbnailbar_item
    width:  (selected) ? (thumbnailbar.previewWidth-10) : (thumbnailbar.itemWidth)
    height: (selected) ? (thumbnailbar.previewHeight - thumbnailbar.itemHeight-10) : (thumbnailbar.itemHeight)
    y:      (selected) ? (thumbnailbar.previewY + dynamic_pad_y+5) : ((thumbnailbar.itemHeight+thumbnailbar.item_padding)*row + thumbnailbar.item_padding + thumbnailbar.list_y)
    x:      (selected) ? (thumbnailbar.previewX + dynamic_pad_x+5) : ((thumbnailbar.itemWidth+thumbnailbar.item_padding)*column + thumbnailbar.item_padding + thumbnailbar.list_x)
    z:      (selected) ? 100 : 0
    smooth: true

    property real dynamic_pad_x: 0
    property real dynamic_pad_y: 0

    property int index: 0
    property int column: (index+index_padding) % thumbnailbar.items_per_line
    property int row:    Math.floor((index+index_padding) / thumbnailbar.items_per_line)

    // @disable-check M126
    property bool highlighted: index == thumbnailbar.highlighted_index
    property bool selected: false
    property bool filmstrip: false

    property bool cached: false
    property bool mouseOver: false

    property string source
    property string internal_source_thumb
    property string internal_source_light
    property string internal_source_hard

    property alias ratio: image.ratio

    property string text
    property string text_little
    property string dir_path
    property string exiv
    property string path

    property int   group: 0
    property color group_color

    property real pic_frame_w: (selected)? width-pad_w_slct  : itemWidth-pad_w
    property real pic_frame_h: (selected)? height-pad_h_slct : itemHeight-pad_h

    property real pad_w_slct: 20
    property real pad_h_slct: 20
    property real pad_w: 10
    property real pad_h: 10

    property alias imageWidth: image.realWidth
    property alias imageHeight: image.realHeight

    property bool linear: (!thumbnailbar.fancy && selected)
    property bool isDir: false

    property real selector_normal_opacity: (highlighted && !selected)? 0.8 : selector_opacity
    property real selector_opacity: 0.1

    function zooming(delta){
        image.zooming(delta)
    }

    function show(){
        if(isDir)
        {

        }
        else
        {
            hide_timer.stop()
            var last_anim_stt = animations_on
            if(main.slide_animations && last_previewed != -1 && Math.abs(index-last_previewed) == 1)
                animations_on = false;

            selector_opacity = 0.5
            thumbnailbar_item.selected = true

            dynamic_pad_x = (last_previewed<index)? thumbnailbar.width : -thumbnailbar.width
            dynamic_pad_y = 0
            animations_on = last_anim_stt;

            dynamic_pad_x = 0
            dynamic_pad_y = 0
        }
    }

    function hide(){
        if(isDir)
        {

        }
        else
        {
            image.zoom = 1
            image.cx = 0
            image.cy = 0

            if(main.slide_animations && on_preview != -1 && Math.abs(index-on_preview) == 1)
            {
                dynamic_pad_x = (on_preview<index)? thumbnailbar.width : -thumbnailbar.width
                dynamic_pad_y = 0
                hide_timer.start()
            }
            else
            {
                selector_opacity = 0.1
                thumbnailbar_item.selected = false
            }
        }
    }


    function click() {

    }

    function doubleClick(){
        if(View.onProcess())
            return;

        if(isDir)
        {
            showDir()
        }
        else
        {

            if(thumbnailbar.on_preview !== -1 && thumbnailbar.on_preview !== index)
                thumbnailbar.fancy = false

            if(thumbnailbar_item.selected)
            {
                thumbnailbar.last_previewed = thumbnailbar.on_preview
                thumbnailbar.on_preview = -1
                thumbnailbar.preview(thumbnailbar.on_preview)
            }

            else
            {
                thumbnailbar.last_previewed = thumbnailbar.on_preview
                thumbnailbar.on_preview = index
                thumbnailbar.preview(thumbnailbar.on_preview)
            }

            thumbnailbar.fancy = true
            thumbnailbar.select(index, 0);
        }
    }

    onFilmstripChanged: {

        if (filmstrip == true) {
            thumbnailbar.last_previewed = thumbnailbar.on_preview
            thumbnailbar.on_preview = index
            thumbnailbar.preview(thumbnailbar.on_preview)
            show()
        }
    }


    function showDir() {
        thumbnailbar.on_preview = -1
        thumbnailbar.openDir(dir_path)
    }

    function crop(x , y , w , h) {
        thumbnailbar_item.cropRequest(x,y,w,h)
    }

    function dselect() {
    }

    onSelectedChanged: {
        if(thumbnailbar.on_preview !== index && thumbnailbar.on_preview !== -1)
            return;

        thumbnailbar.on_preview = (selected)?index:-1
    }

    Timer {
        id: double_click_timer
        interval: 173
        running: false
        repeat: false
        onTriggered: {
            if(image.zoom !== 1)
                return

            click()
        }
    }

    Timer {
        id: hide_timer
        interval: x_anim_speed
        running: false
        repeat: false
        onTriggered: {
            var last_anim_stt = animations_on
            animations_on = false
            dynamic_pad_x = 0
            dynamic_pad_y = 0
            selector_opacity = 0.1
            thumbnailbar_item.selected = false
            animations_on = last_anim_stt
        }
    }

    Connections{
        target: thumbnailbar
        onPreview: {
            if(thumbnailbar.on_preview === thumbnailbar_item.index && isDir)
            {
                thumbnailbar.on_preview = -1
                return
            }

            if(thumbnailbar.on_preview === thumbnailbar_item.index)
                show()
            else if(selected)
                hide()
        }
    }

    Connections{
        target: thumbnailbar
        onRotateLeft: {
            if(!selected)
                return

            dselect()

        }
    }

    Connections{
        target: thumbnailbar
        onRotateRight: {
            if(!selected)
                return

            dselect()

        }
    }

    Connections{
        target: View
        onWheel: {
            if(on_preview === -1 || !main.scroll_block)
                return
        }
    }

    property real x_anim_speed: (thumbnailbar_item.selected)? 437 : 400*Math.random() +437
    property real y_anim_speed: (thumbnailbar_item.selected)? 437 : 400*Math.random() +437
    property real w_anim_speed: (thumbnailbar_item.selected)? 437 : 400*Math.random() +437
    property real h_anim_speed: (thumbnailbar_item.selected)? 437 : 400*Math.random() +437

    Behavior on x {
        NumberAnimation { easing.type: (linear)?Easing.Linear:easing_type; duration: (animations_on && !disable_animations)? x_anim_speed : 0 }
    }
    Behavior on y {
        NumberAnimation { easing.type: (linear)?Easing.Linear:easing_type; duration: (animations_on && !disable_animations)? y_anim_speed : 0 }
    }
    Behavior on width {
        NumberAnimation { easing.type: (linear)?Easing.Linear:easing_type; duration: (animations_on && !disable_animations)? w_anim_speed : 0 }
    }
    Behavior on height {
        NumberAnimation { easing.type: (linear)?Easing.Linear:easing_type; duration: (animations_on && !disable_animations)? h_anim_speed : 0 }
    }

    Rectangle {
        id: selector
        anchors.fill: parent
        color: (isDir) ? "grey" : group_color
        border.color: "black"
        border.width: 3
        radius: 0
        opacity: unselected_opacity
        smooth: thumbnailbar_item.smooth

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.Linear; duration: (animations_on && !disable_animations)? 537 : 0 }
        }
        Behavior on color {
            ColorAnimation { easing.type: Easing.OutQuint; duration: (animations_on && !disable_animations)? 1235 : 0 }
        }

        property string selected_color: "#222222"
        property real unselected_opacity: (cached && !selected && !mouseOver)? 0.4 : selector_normal_opacity

    }

    ZoomImage {
        id: image
        x: (parent.width - width)/2 + cx
        y: (parent.height - height)/2 + cy

        source: src

        property string src: (thumbnailbar_item.selected || isDir)? image_src : thumbnail_src
        property real cx: 0
        property real cy: 0

        property real rotate

        property string thumbnail_src: thumbnailbar_item.internal_source_thumb
        property string image_src: (isDir)? thumbnailbar_item.source : internal_source
        property string internal_source: (zoom <= 1)? thumbnailbar_item.internal_source_light : thumbnailbar_item.internal_source_hard

        rotation: (selected)? rotate : rotate

        onZoomChanged:{
            if(zoom > 1)
                return

            cx = 0
            cy = 0
        }

        Behavior on rotation {
            NumberAnimation { easing.type: (linear)?Easing.Linear:easing_type; duration: (animations_on && !disable_animations && selected)? 400*Math.random() +437 : 0 }
        }
    }

    Text {
        id: label
        font.bold: true
        font.pointSize: 10
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        visible: (thumbnailbar_item.selected) || isDir
        color: (thumbnailbar_item.selected)? "#000000" : "#333333"

        text: (thumbnailbar_item.selected)? thumbnailbar_item.text : thumbnailbar_item.text_little
    }

    MouseArea {
        id: mousearea
        hoverEnabled: true
        anchors.fill: (selected)? image : parent
        rotation: (selected && image.zoom<=1)? image.rotation : 0

        property real gMouseX
        property real gMouseY

        onMouseXChanged: {
            gMouseX = mousearea.mapToItem(thumbnailbar_item,mouseX,mouseY).x
            if(!pressed)
            {
                last_mouse_x = mouseX
                return
            }

            if(!confirm_move_x)
            {
                confirm_move_x = true
                return
            }

            if(image.zoom > 1)
            {
                image.cx += mouseX - last_mouse_x
                last_mouse_x = mouseX
            }
        }

        onMouseYChanged: {
            gMouseY = mousearea.mapToItem(thumbnailbar_item,mouseX,mouseY).y
            if(!pressed)
            {
                last_mouse_y = mouseY
                return
            }

            if(!confirm_move_y)
            {
                confirm_move_y = true
                return
            }

            if(image.zoom > 1)
            {
                image.cy += mouseY - last_mouse_y
                last_mouse_y = mouseY
            }
        }

        onEntered: {
            if(!thumbnailbar_item.selected && selector_opacity !== 1.0)
                selector_opacity = 0.8
            if(thumbnailbar_item.selected)
                main.scroll_block = true

            thumbnailbar_item.mouseOver = true
        }

        onExited: {
            if(selector_opacity === 0.8)
                selector_opacity = 0.2
            if(thumbnailbar_item.selected)
                main.scroll_block = false

            thumbnailbar_item.mouseOver = false
        }

        onPressed: {
            if(!thumbnailbar_item.selected)
                selector_opacity = 0.9
        }

        onReleased: {
            confirm_move_x = false
            confirm_move_y = false
        }


        onClicked: {
            if(View.onProcess())
                return;

            if(isDir)
            {
                return;
            }

            // Filmstrip view, only allow one select
            if(thumbnailbar.on_preview !== -1 && thumbnailbar_item.selected !== -1)
            {
                thumbnailbar.last_previewed = thumbnailbar.on_preview
                thumbnailbar.on_preview = index
                thumbnailbar.preview(thumbnailbar.on_preview)
                thumbnailbar.fancy = true
                thumbnailbar.select(index, 0);
                return;
            }

            // Pass any clicks with modifiers via select signal, so code can handle selects
            var modifier = 0;
            if (mouse.modifiers === Qt.AltModifier)
            {
                modifier = 1;
            }
            if (mouse.modifiers === Qt.ControlModifier)
            {
                modifier = 2;
            }
            if (mouse.modifiers === Qt.ShiftModifier)
            {
                modifier = 3;
            }

            thumbnailbar.select(index, modifier);
        }


        onDoubleClicked: {
            doubleClick()
        }

        property real last_mouse_x: 0
        property real last_mouse_y: 0
        property bool confirm_move_x: false
        property bool confirm_move_y: false
    }
}
