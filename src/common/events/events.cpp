#include "urf/common/events/events.hpp"

namespace urf {
namespace common {
namespace events {

threading::ThreadPool event_base::_threadPool(1);

} // namespace events
} // namespace common
} // namespace urf