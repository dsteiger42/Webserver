#!/bin/bash

BASE="http://localhost:8080"

echo "[1] Upload"
curl -s -X POST -F "file=@/tmp/shell.py" $BASE/upload/

echo "[2] List uploads (se autoindex estiver on)"
curl $BASE/uploads/

echo "[3] Testar ficheiro gerado manualmente"
FILE=$(ls www/uploads | tail -n 1)

echo "Testing: $FILE"
curl $BASE/uploads/$FILE