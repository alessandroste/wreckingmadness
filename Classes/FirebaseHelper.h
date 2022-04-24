#ifndef _FIREBASEHELPER_H_
#define _FIREBASEHELPER_H_

#include <memory>
#ifdef FIREBASE
#include "firebase/app.h"
#include "firebase/admob/types.h"
#endif

namespace wreckingmadness {
    class FirebaseHelper
    {
    private:
#ifdef FIREBASE
        std::unique_ptr<firebase::App> _pFirebaseApp;
        firebase::admob::AdParent getAdParent();
#endif
    public:
        FirebaseHelper();
        ~FirebaseHelper();
    };
}

#endif