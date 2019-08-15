#!/bin/bash
cd `dirname $0`
grep -r --include="*.build" --include="*.cpp" --include="*.hpp" "TODO" "include" "src" "test"
