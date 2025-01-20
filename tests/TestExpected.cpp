#include "utils/Expected.hpp"

#include <string>

int
main()
{
    utils::Expected<int, std::string> exp;
    assert(exp.HasValue());
    assert(exp.Value() == 0);

    exp = 10;
    assert(exp.HasValue());
    assert(exp.Value() == 10);

    exp = utils::UnExpected{ "foo" };
    assert(!exp.HasValue());
    assert(exp.Error() == "foo");
}
