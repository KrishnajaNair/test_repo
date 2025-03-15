#include <mts/extensibility/data_description_parser.h>
#include <mts/extensibility/data_matcher.h>
#include <mts/extensibility/data_source.h>
#include <mts/extensibility/dynamic_data.h>
#include <mts/extensibility/formatter.h>
#include <mts/extensibility/io.h>
#include <mts/extensibility/package.h>
#include <mts/extensibility/primitives_sequence.h>
#include <mts/extensibility/raw_package.h>
#include <mts/extensibility/reader.h>
#include <mts/extensibility/service_locator.h>
#include <mts/extensibility/signal_storage.h>
#include <mts/extensibility/sources.h>
#include <mts/extensibility/string.h>
#include <mts/extensibility/time_spec.h>
#include <mts/extensibility/type_node_group.h>
#include <mts/extensibility/type_node_signal.h>
#include <mts/extensibility/type_node_subgroup.h>
#include <mts/extensibility/type_node_view.h>
#include <mts/extensibility/type_node.h>
#include <mts/extensibility/udex_extractor.h>
#include <mts/runtime/allocator.h>
#include <mts/runtime/endian.h>
#include <mts/runtime/error.h>
#include <mts/runtime/exit_codes.h>
#include <mts/runtime/log.h>
#include <mts/runtime/runtime.h>
#include <mts/runtime/string.h>
#include <mts/runtime/types.h>

int main()
{
    mts_service_locator_t *service_locator = mts_get_default_service_locator();
    (void)service_locator;
    return 0;
}