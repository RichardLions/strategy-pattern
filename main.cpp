#include <catch2/catch_session.hpp>

#include "referencesemantics_examples.h"
#include "template_examples.h"
#include "valuesemantics_examples.h"

int main(const int argc, const char* const argv[])
{
    return Catch::Session().run(argc, argv);
}
