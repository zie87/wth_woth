#include "PrecompiledHeader.h"

#include "ObjectAnalytics.h"

#include "CardPrimitive.h"
#include "ExtraCost.h"
#include "ManaCost.h"
#include "MTGCard.h"
#include "MTGCardInstance.h"


#include <wge/log.hpp>

namespace ObjectAnalytics {
void DumpStatistics() {
#ifdef TRACK_OBJECT_USAGE

    WGE_LOG_TRACE("-----------------------------------------------------------");
    WGE_LOG_TRACE("Object Usage Stats");
    WGE_LOG_TRACE("CardPrimitive current count: {}", InstanceCounter<CardPrimitive>::GetCurrentObjectCount());
    WGE_LOG_TRACE("CardPrimitive current byte usage: {}", InstanceCounter<CardPrimitive>::GetCurrentByteCount());
    WGE_LOG_TRACE("CardPrimitive max count: {}", InstanceCounter<CardPrimitive>::GetMaximumObjectCount());
    WGE_LOG_TRACE("CardPrimitive max byte usage: {}", InstanceCounter<CardPrimitive>::GetMaximumByteCount());
    WGE_LOG_TRACE("-----------------------------------------------------------");
    WGE_LOG_TRACE("MTGCard current count: {}", InstanceCounter<MTGCard>::GetCurrentObjectCount());
    WGE_LOG_TRACE("MTGCard current byte usage: {}", InstanceCounter<MTGCard>::GetCurrentByteCount());
    WGE_LOG_TRACE("MTGCard max count: {}", InstanceCounter<MTGCard>::GetMaximumObjectCount());
    WGE_LOG_TRACE("MTGCard max byte usage: {}", InstanceCounter<MTGCard>::GetMaximumByteCount());
    WGE_LOG_TRACE("-----------------------------------------------------------");
    WGE_LOG_TRACE("MTGCardInstance current count: {}", InstanceCounter<MTGCardInstance>::GetCurrentObjectCount());
    WGE_LOG_TRACE("MTGCardInstance current byte usage: {}", InstanceCounter<MTGCardInstance>::GetCurrentByteCount());
    WGE_LOG_TRACE("MTGCardInstance max count: {}", InstanceCounter<MTGCardInstance>::GetMaximumObjectCount());
    WGE_LOG_TRACE("MTGCardInstance max byte usage: {}", InstanceCounter<MTGCardInstance>::GetMaximumByteCount());
    WGE_LOG_TRACE("-----------------------------------------------------------");
    WGE_LOG_TRACE("ManaCost current count: {}", InstanceCounter<ManaCost>::GetCurrentObjectCount());
    WGE_LOG_TRACE("ManaCost current byte usage: {}", InstanceCounter<ManaCost>::GetCurrentByteCount());
    WGE_LOG_TRACE("ManaCost max count: {}", InstanceCounter<ManaCost>::GetMaximumObjectCount());
    WGE_LOG_TRACE("ManaCost max byte usage: {}", InstanceCounter<ManaCost>::GetMaximumByteCount());
    WGE_LOG_TRACE("-----------------------------------------------------------");
    WGE_LOG_TRACE("ExtraCost current count: {}", InstanceCounter<ExtraCost>::GetCurrentObjectCount());
    WGE_LOG_TRACE("ExtraCost current byte usage: {}", InstanceCounter<ExtraCost>::GetCurrentByteCount());
    WGE_LOG_TRACE("ExtraCost max count: {}", InstanceCounter<ExtraCost>::GetMaximumObjectCount());
    WGE_LOG_TRACE("ExtraCost max byte usage: {}", InstanceCounter<ExtraCost>::GetMaximumByteCount());
    WGE_LOG_TRACE("-----------------------------------------------------------");

#endif
}
}  // namespace ObjectAnalytics
