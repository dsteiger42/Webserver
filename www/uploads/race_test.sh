#!/bin/bash

URL="http://localhost:8080/upload/test.txt"
FILE="./www/uploads/test.txt"

rm -f $FILE

echo "[+] A testar race condition real..."

# guardar resultados
RESULTS=$(mktemp)

for i in $(seq 1 100); do
(
    echo "payload_$i" | curl -s -o /dev/null -w "%{http_code}\n" \
        -X POST "$URL" --data-binary @-
) >> $RESULTS &
done

wait

echo "[+] Resultados:"
cat $RESULTS | sort | uniq -c

echo "[+] Conteúdo final:"
cat $FILE 2>/dev/null || echo "ficheiro não existe"
