SET(SOURCES ui_Screen3.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_img_background_img_w_gradient_png.c
    ui_img_gauge_empty_png.c
    ui_img_gauge_full_png.c)

add_library(ui ${SOURCES})
