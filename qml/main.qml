import QtQuick 1.1

Rectangle {
    id: main
    color: "#00000000"
    smooth: false
    width: 500
    height: 200

    property real back_opacity
    property alias linear: thumbnailbar.linear
    property bool scroll_block: false

    property int minimum_cache_size: Settings.value("general/minimum_cache_size" , 1)
    property int maximum_cache_size: Settings.value("general/maximum_cache_size" , 2)

    property int thumbnails_load_pad: 500

    property bool disable_animations: Settings.value("general/disable_animations" , false)
    property bool slide_animations: Settings.value("general/slide_animation" , true)

    property real preview_size: Settings.value("general/preview_size", 50)  * 160 / 50
    property real small_preview_size: Settings.value("general/small_preview_size" , 50) * 92 / 50

    // @disable-check M311
    property variant easing_type: Easing.OutExpo

    ScrollBar {
        id: scrollbar
        objectName: "scrollbar"

        // TODO: Re-calculate size for large icons. Scroller goes beyond view for large sizes of icons
        x: (thumbnailbar.linear)? 0 : main.width - width
        y: (thumbnailbar.linear)? main.height - height : 0
        width: (thumbnailbar.linear)? main.width : scrollerSize
        height: (thumbnailbar.linear)? scrollerSize : main.height

        horizontal: thumbnailbar.linear

        scrollerWidth: (thumbnailbar.linear)? main.width*main.width/thumbnailbar.list_width : scrollerSize
        scrollerHeight: (thumbnailbar.linear)? scrollerSize : main.height*main.height/thumbnailbar.list_height

        onScrollerXChanged: {
            checkX()
        }
        onScrollerYChanged: {
            checkY()
        }

        Connections{
            target: View
            onWheel:{
                if(main.scroll_block)
                    return

                if(scrollbar.horizontal)
                    scrollbar.scrollerX += (-1*delta/120)*(thumbnailbar.itemWidth) * width / thumbnailbar.list_width;
                else
                    scrollbar.scrollerY += (-1*delta/120)*(thumbnailbar.itemHeight) * height / thumbnailbar.list_height;
            }
        }

        Connections{
            target: thumbnailbar
            onPreview: {
                if(thumbnailbar.on_preview === -1)
                    return
                if(!scrollbar.horizontal)
                    return

                scrollbar.scrollerX = ((thumbnailbar.itemWidth+thumbnailbar.item_padding)*thumbnailbar.on_preview-width/2+thumbnailbar.itemWidth/2) * width / thumbnailbar.list_width
            }
        }

        Connections{
            target: thumbnailbar
            onHighlighted_rowChanged: {
                if(thumbnailbar.on_preview != -1)
                    return
                if(scrollbar.horizontal)
                    return
                if(thumbnailbar.highlighted_column <= -1 || thumbnailbar.highlighted_row <= -1 || thumbnailbar.highlighted_index >= thumbnailbar.count)
                    return

                if(View.objectAt(thumbnailbar.highlighted_index).y < 0)
                    scrollbar.scrollerY = ((thumbnailbar.itemHeight+thumbnailbar.item_padding)*thumbnailbar.highlighted_row) * height / thumbnailbar.list_height
                else if(View.objectAt(thumbnailbar.highlighted_index).y > height - thumbnailbar.itemHeight)
                    scrollbar.scrollerY = ((thumbnailbar.itemHeight+thumbnailbar.item_padding)*thumbnailbar.highlighted_row-height+thumbnailbar.itemHeight) * height / thumbnailbar.list_height
            }
        }

        color: "#000000"
    }

    ThumbnailBar{
        id: thumbnailbar
        objectName: "thumbnailbar"
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left

        anchors.leftMargin: 4
        anchors.rightMargin: 4
        anchors.topMargin: 4
        anchors.bottomMargin: 4

        onOn_previewChanged: {
            if(on_preview === -1)
                scroll_block = false
            else
                scroll_block = true
        }

        list_x: (linear)? -1*scrollbar.scrollerX*list_width/scrollbar.width : 0
        list_y: (linear)? height-itemHeight : -1*scrollbar.scrollerY*list_height/scrollbar.height
    }

    focus: true

    Keys.onEscapePressed: {

    }

    Keys.onLeftPressed: {
        if(thumbnailbar.on_preview != -1)
        {
            thumbnailbar.previous()
        }
        else
        {
            var tmp = thumbnailbar.animations_on
            thumbnailbar.animations_on = false

            thumbnailbar.highlighted_column--;
            if(thumbnailbar.highlighted_column < 0)
                thumbnailbar.highlighted_column++;
            if(thumbnailbar.highlighted_row == -1)
                thumbnailbar.highlighted_row = 0;

            thumbnailbar.animations_on = tmp
        }
    }

    Keys.onRightPressed: {
        if(thumbnailbar.on_preview != -1)
        {
            thumbnailbar.next()
        }
        else
        {
            var tmp = thumbnailbar.animations_on
            thumbnailbar.animations_on = false

            thumbnailbar.highlighted_column++;
            if(thumbnailbar.highlighted_column >= thumbnailbar.items_per_line || thumbnailbar.highlighted_index >= thumbnailbar.count)
                thumbnailbar.highlighted_column--;
            if(thumbnailbar.highlighted_row == -1)
                thumbnailbar.highlighted_row = 0;

            thumbnailbar.animations_on = tmp
        }
    }
    Keys.onUpPressed: {
        if(thumbnailbar.on_preview != -1)
        {
        }
        else
        {
            var tmp = thumbnailbar.animations_on
            thumbnailbar.animations_on = false

            thumbnailbar.highlighted_row--;
            if(thumbnailbar.highlighted_row < 0)
                thumbnailbar.highlighted_row++;
            if(thumbnailbar.highlighted_column == -1)
                thumbnailbar.highlighted_column = 0;

            thumbnailbar.animations_on = tmp
        }
    }
    Keys.onDownPressed: {
        if(thumbnailbar.on_preview != -1)
        {
        }
        else
        {
            var tmp = thumbnailbar.animations_on
            thumbnailbar.animations_on = false

            thumbnailbar.highlighted_row++;
            if(thumbnailbar.highlighted_row >= thumbnailbar.rows_number || thumbnailbar.highlighted_index >= thumbnailbar.count)
                thumbnailbar.highlighted_row--;
            if(thumbnailbar.highlighted_column == -1)
                thumbnailbar.highlighted_column = 0;

            thumbnailbar.animations_on = tmp
        }
    }

    Keys.onEnterPressed: {
        if(thumbnailbar.on_preview != -1)
        {
            // call to save?
            thumbnailbar.next();
        }
        View.objectAt(thumbnailbar.highlighted_index).doubleClick()
    }

    Keys.onReturnPressed: {
        if(thumbnailbar.on_preview != -1)
        {
            thumbnailbar.next();
        }
        else
        {
            if(thumbnailbar.highlighted_index == -1)
            {
                for(var i=0 ; i<thumbnailbar.count ; i++)
                    if(!View.objectAt(i).isDir)
                    {
                        thumbnailbar.show(i)
                        break;
                    }
            }
            else
                View.objectAt(thumbnailbar.highlighted_index).doubleClick()
        }
    }

    Keys.onPressed: {
        if((event.key === Qt.Key_Plus || event.key === Qt.Key_Equal) && thumbnailbar.on_preview != -1){
            View.objectAt(thumbnailbar.on_preview).zooming(120)
        }
        if((event.key === Qt.Key_Minus) && thumbnailbar.on_preview != -1){
            View.objectAt(thumbnailbar.on_preview).zooming(-120)
        }
    }

}
