// RUN: polyqsim-opt %s | polyqsim-opt | FileCheck %s

module {
    // CHECK-LABEL: func @bar()
    func.func @bar() {
        %0 = arith.constant 1 : i32
        // CHECK: %{{.*}} = polyqsim.foo %{{.*}} : i32
        %res = polyqsim.foo %0 : i32
        return
    }
}
