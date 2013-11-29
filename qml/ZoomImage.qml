import QtQuick 1.1

Image {
    width:  w*zoom
    height: h*zoom
    smooth: true
    fillMode: Image.Stretch
    asynchronous: true
    cache: false
    sourceSize.width: realWidth
    sourceSize.height: realHeight

    property real w: (realHeight<iheight && realWidth <iwidth)? realWidth : iwidth
    property real h: (realHeight<iheight && realWidth <iwidth)? realHeight : iheight

    property real iwidth:  (!selected)? normal_iwidth : rotated_iwidth
    property real iheight: (!selected)? normal_iheight : rotated_iheight

    property real ratio: 1
    property real zoom: 1

    property real realHeight
    property real realWidth

    property real normal_iwidth:  (ratio>=pic_frame_w/pic_frame_h)? pic_frame_w + small_for_update        : pic_frame_h*ratio + small_for_update
    property real normal_iheight: (ratio>=pic_frame_w/pic_frame_h)? pic_frame_w/ratio + small_for_update  : pic_frame_h + small_for_update

    property real rotated_iwidth:  (ratio>=pic_frame_h/pic_frame_w)? pic_frame_h + small_for_update       : pic_frame_w*ratio + small_for_update
    property real rotated_iheight: (ratio>=pic_frame_h/pic_frame_w)? pic_frame_h/ratio + small_for_update : pic_frame_w + small_for_update

    Connections{
        target: View
        onWheel:{
            if(!thumbnailbar_item.selected)
                return

            zooming(delta)
        }
    }

    function zooming(delta)
    {
        thumbnailbar_item.dselect()
        zoom += delta/1200
        if(zoom < 1) zoom = 1
    }

    Keys.onNoPressed: {
        if (event.key === Qt.Key_Plus) zoom += 1;
        if (event.key === Qt.Key_Minus) zoom -= 1;
        if (zoom < 1) zoom = 1;
    }

    property real small_for_update: 0
    onRotationChanged: smallUpdateForQmlBug()
    Connections{
        target:thumbnailbar_item
        onWidthChanged:  smallUpdateForQmlBug()
        onHeightChanged: smallUpdateForQmlBug()
    }

    function smallUpdateForQmlBug() {
        if(!thumbnailbar_item.selected && rotation == 0)
            return
        if(small_for_update == 0)
            small_for_update = 0.00000000001
        else
            small_for_update = 0
    }
}
