#pragma once

#include "qjs++/impl/ClosureClass.h"
#include "qjs++/impl/Exception-decl.h"
#include "qjs++/impl/traits/JSValueTraits.h"
#include <cassert>
#include <mutex>
#include <quickjs.h>

namespace qjs {

// struct ClosureData {
//   JSCallback callback = nullptr;
//   JSCallbackFinalizer finalizer = nullptr;
//   void *opaque = nullptr;
// };
//
// static JSClassID kClosureClassId = 0;

// inline static void CreateClosureClass(JSContext *ctx) {
//   static std::once_flag once;

//  std::call_once(
//      once, [ctx]() { JS_NewClassID(JS_GetRuntime(ctx), &kClosureClassId); });

//  static JSClassDef pointer{"ClosureClass"};

//  pointer.finalizer = [](JSRuntime *rt, JSValue val) {
//    auto *cd = static_cast<ClosureData *>(JS_GetOpaque(val, kClosureClassId));
//    assert(cd);
//    if (cd) {
//      if (cd->finalizer) {
//        cd->finalizer(cd->opaque);
//      }
//      delete cd;
//    }
//  };
//  JS_NewClass(JS_GetRuntime(ctx), kClosureClassId, &pointer);
//}

} // namespace qjs
