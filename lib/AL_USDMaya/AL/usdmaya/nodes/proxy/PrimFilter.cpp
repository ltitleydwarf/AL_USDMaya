#include "AL/usdmaya/nodes/ProxyShape.h"
#include "AL/usdmaya/nodes/proxy/PrimFilter.h"
#include "AL/usdmaya/fileio/SchemaPrims.h"

namespace AL {
namespace usdmaya {
namespace nodes {
namespace proxy {

//----------------------------------------------------------------------------------------------------------------------
PrimFilter::PrimFilter(const SdfPathVector& previousPrims, const std::vector<UsdPrim>& newPrimSet, PrimFilterInterface* proxy)
  : m_newPrimSet(newPrimSet), m_transformsToCreate(), m_updatablePrimSet(), m_removedPrimSet()
{
  // copy over original prims
  m_removedPrimSet.assign(previousPrims.begin(), previousPrims.end());
  std::sort(m_removedPrimSet.begin(), m_removedPrimSet.end(),  [](const SdfPath& a, const SdfPath& b){ return b < a; } );

  // to see if no prims are found
  TfToken nullToken;

  for(auto it = m_newPrimSet.begin(); it != m_newPrimSet.end(); )
  {
    UsdPrim prim = *it;
    auto lastIt = it;
    ++it;

    SdfPath path = prim.GetPath();

    // check previous prim type (if it exists at all?)
    TfToken type = proxy->getTypeForPath(path);
    TfToken newType = prim.GetTypeName();

    bool supportsUpdate = false;
    bool requiresParent = false;
    proxy->getTypeInfo(newType, supportsUpdate, requiresParent);

    if(nullToken == type)
    {
      // all good, prim will remain in the new set (we have no entry for it)
      if(requiresParent)
      {
        m_transformsToCreate.push_back(prim);
      }
    }
    else
    {
      // if the type remains the same, and the type supports update
      if(supportsUpdate)
      {
        if(type == prim.GetTypeName())
        {
          // add to updatable prim list
          m_updatablePrimSet.push_back(prim);

          // locate the path and delete from the removed set (we do not want to delete this prim!
          auto iter = std::lower_bound(m_removedPrimSet.begin(), m_removedPrimSet.end(), path, [](const SdfPath& a, const SdfPath& b){ return b < a; } );
          if(iter != removedPrimSet().end() && *iter == path)
          {
            m_removedPrimSet.erase(iter);
            it = m_newPrimSet.erase(lastIt);
          }
        }
      }
      else
      {
        // if we need a transform, make a note of it now
        if(requiresParent)
        {
          m_transformsToCreate.push_back(prim);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // proxy
} // nodes
} // usdmaya
} // AL
//----------------------------------------------------------------------------------------------------------------------
