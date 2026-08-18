#include <cstdlib>
#include <iostream>
#include <string>

#include "stringutils.h"

int main()
{
    if (StringUtils::BaseName("parent/child\\image.bif") != "image.bif"
        || StringUtils::BaseName("parent\\child/image.bif") != "image.bif"
        || StringUtils::FolderPath("parent/child\\image.bif") != "parent/child"
        || StringUtils::FolderPath("parent\\child/image.bif") != "parent\\child") {
        std::cerr << "mixed path separators were not handled consistently\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
