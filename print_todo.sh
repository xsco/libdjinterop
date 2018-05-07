#!/bin/bash
cd `dirname $0`
grep -r --include="*.am" --include="*.ac" --include="*.cpp" --include="*.hpp" "TODO" src/
