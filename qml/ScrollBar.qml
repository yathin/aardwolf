import QtQuick 1.1

Item {
    id: scrollbar

    property alias color: scroller.color

    property real scrollerX:0
    property real scrollerY:0
    property real scrollerWidth:  scrollerSize
    property real scrollerHeight: scrollerSize

    property real scrollerSize: 16

    property bool horizontal: false
    property bool blocker: false

    onScrollerXChanged: {
        checkX()
    }
    onScrollerYChanged: {
        checkY()
    }

    function checkX(){
        if(blocker)
            return

        blocker = true
        if(scrollbar.scrollerX < 0)
            scrollbar.scrollerX = 0
        else if(scrollbar.scrollerX + scrollbar.scrollerWidth > scrollbar.width)
            scrollbar.scrollerX = scrollbar.width - scrollbar.scrollerWidth
        blocker = false
    }

    function checkY(){
        if(blocker)
            return

        blocker = true
        if(scrollbar.scrollerY < 0)
            scrollbar.scrollerY = 0
        else if(scrollbar.scrollerY + scrollbar.scrollerHeight > scrollbar.height)
            scrollbar.scrollerY = scrollbar.height - scrollbar.scrollerHeight
        blocker = false
    }

    Rectangle {
        id: scroller
        radius: 15
        smooth: true
        opacity: 0.3

        x: (scrollbar.horizontal)? scrollbar.scrollerX : 0
        y: (scrollbar.horizontal)? 0 : scrollbar.scrollerY
        width: (scrollbar.horizontal)? scrollbar.scrollerWidth : scrollerSize
        height: (scrollbar.horizontal)? scrollerSize : scrollbar.scrollerHeight

        Behavior on opacity {
            NumberAnimation { easing.type: Easing.Linear; duration: (disable_animations)? 0 : 357 }
        }

        MouseArea{
            id: mousearea
            hoverEnabled: true
            anchors.fill: scroller

            onEntered: scroller.opacity = 0.7
            onExited:  scroller.opacity = 0.3

            onMouseXChanged: {
                if(!scrollbar.horizontal)
                    return
                if(!pressed)
                {
                    last_mouse_x = mouseX
                    return
                }

                scrollbar.scrollerX += mouseX - last_mouse_x
            }

            onMouseYChanged: {
                if(scrollbar.horizontal)
                    return
                if(!pressed)
                {
                    last_mouse_y = mouseY
                    return
                }

                scrollbar.scrollerY += mouseY - last_mouse_y
            }

            property real last_mouse_x: 0
            property real last_mouse_y: 0
        }
    }
}
