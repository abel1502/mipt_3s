#ifndef AGF_WIDGETS_GROUP_H
#define AGF_WIDGETS_GROUP_H

#include <AGF/widget.h>
#include <ACL/list.h>


namespace abel::gui::widgets {


class Group : public virtual Widget {
public:
    //

    ~Group() = default;

protected:
    list<unique_ptr<Widget>> children{};

};


}


#endif // AGF_WIDGETS_BUTTON_H

