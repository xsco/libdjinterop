#include <djinterop/enginelibrary/el_storage.hpp>

namespace djinterop
{
namespace enginelibrary
{
el_storage::el_storage(std::string directory)
    : directory{directory}, db{":memory:"}
{
    // TODO (haslersn): Should we check that directory is an absolute path?
    db << "ATTACH ? as 'music'" << (directory + "/m.db");
    db << "ATTACH ? as 'perfdata'" << (directory + "/p.db");
}

}  // namespace enginelibrary
}  // namespace djinterop
