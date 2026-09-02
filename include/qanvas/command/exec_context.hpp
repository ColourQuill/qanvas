#ifndef __QANVAS_EXEC_CONTEXT_HPP__
#define __QANVAS_EXEC_CONTEXT_HPP__

namespace Qore::Qanvas {
    class Device;

    struct ExecContext {
        Device* device;
    }; 
}

#endif