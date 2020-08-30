# The idea of this test is to ensure no new DLL dependency is added involuntarly.
# All needed DLLs should be included in the archive.
#
# If the test fails, the package needs to be verified manually under windows or
# wine to see if a DLL needs to be added to it. If so, it needs to be added to
# the DLLS variable in makefile.
ARCH=$1
ACTUAL=$(mktemp)
EXPECTED=$(mktemp)

if [ "${ARCH}" == "win32" ]
then
  echo "Skipping test for ${ARCH}"
  exit 0
fi

# All dependencies should be resolved in the ./win/ directory. If anything is in
# mingw, that's a missing dependency.
cat << EOF > ${EXPECTED}
EOF

echo "ldd output:"
ldd win/kalenji_reader.exe

ldd win/kalenji_reader.exe | grep mingw | sed 's/ =>.*//' | sed 's/^[ \t]*//' | sort > ${ACTUAL}

echo "mingw dependencies:"
cat "${ACTUAL}"

if ! diff ${ACTUAL} ${EXPECTED}
then
  echo "!! dependencies test for ${ARCH} failed"
  exit 1
else
  echo "The test was successful!"
fi
