/*
The MIT License (MIT)

Copyright (c) 2013 Stephan Brenner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
// Original:
// https://github.com/stbrenner/gtest_mem/blob/master/gtest_mem_main.cpp

#include <gtest/gtest.h>

#ifdef _MSC_VER

#include <crtdbg.h>

class WindowsMemoryLeakDetector : public testing::EmptyTestEventListener {
 public:
  void OnTestStart(const testing::TestInfo&) override {
    _CrtMemCheckpoint(&memState_);
  }

  void OnTestEnd(const testing::TestInfo& test_info) override {
    if (test_info.result()->Passed()) {
      _CrtMemState stateNow, stateDiff;
      _CrtMemCheckpoint(&stateNow);
      int diffResult = _CrtMemDifference(&stateDiff, &memState_, &stateNow);
      if (diffResult) {
        FAIL() << "Memory leak of " << stateDiff.lSizes[1]
               << " byte(s) detected.";
      }
    }
  }

 private:
  _CrtMemState memState_;
};
#endif

GTEST_API_ int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);

#ifdef _MSC_VER
  // gtest takes ownership/
  testing::UnitTest::GetInstance()->listeners().Append(
      new WindowsMemoryLeakDetector);
#endif

  return RUN_ALL_TESTS();
}