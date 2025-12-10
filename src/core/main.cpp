#include "core/menu_controller.h"
#include <iostream>

int main() {
    try {
        MenuController menu;
        menu.showMainMenu();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
