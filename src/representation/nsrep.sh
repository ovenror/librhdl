#!/bin/bash
for f in */*.h structural/simplified/*.h; do sed -i -e 's/^class [[:alpha:]]* [{:]/namespace rep {\n&/' -e 's/^};/&\n}/' $f; done
for f in */*.cpp structural/simplified/*.cpp; do sed -i -e 's/^namespace rhdl {/namespace rhdl::rep {/' $f; done
for f in structural/builder/*.h; do sed -i -e 's/^class [[:alpha:]]* [{:]/namespace rep::sb {\n&/' -e 's/^};/&\n}/' $f; done
for f in structural/builder/*.cpp; do sed -i -e 's/^namespace rhdl {/namespace rhdl::rep::sb {/' -e '/^namespace sb {/d' $f; done

