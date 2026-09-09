#!/bin/bash

PASS=0
FAIL=0
TOTAL=0

roda_teste(){
    TOTAL=$((TOTAL + 1))
    descricao="$1"
    esperado="$2"
    resultado="$3"

    if [ "$esperado" = "$resultado" ]; then
        PASS=$((PASS + 1))
        echo "PASS: $descricao"
    else
        FAIL=$((FAIL + 1))
        echo "FAIL: $descricao"
        echo "  esperado: '$esperado'"
        echo "  resultado: '$resultado'"
    fi
}

echo "=== Happy path: rate voo.txt ==="
./scheduler rate voo.txt > /dev/null 2>&1
diff -q rate_pvcb.out testes/esperado_rate_voo.out > /dev/null 2>&1
roda_teste "rate voo.txt == esperado" "0" "$?"

echo ""
echo "=== Happy path: edf voo.txt ==="
./scheduler edf voo.txt > /dev/null 2>&1
diff -q edf_pvcb.out testes/esperado_edf_voo.out > /dev/null 2>&1
roda_teste "edf voo.txt == esperado" "0" "$?"

echo ""
echo "=== Happy path: rate exemplopvcb.txt ==="
./scheduler rate exemplopvcb.txt > /dev/null 2>&1
# verifica que B perdeu deadline
grep -q "\[B\] 1" rate_pvcb.out || grep -q "\[B\] 2" rate_pvcb.out
roda_teste "rate exemplopvcb.txt tem B perdendo" "0" "$?"

echo ""
echo "=== Happy path: edf exemplopvcb.txt ==="
./scheduler edf exemplopvcb.txt > /dev/null 2>&1
# verifica que nenhum perdeu
grep -q "\[A\] 0" edf_pvcb.out && grep -q "\[B\] 0" edf_pvcb.out
roda_teste "edf exemplopvcb.txt sem perdas" "0" "$?"

echo ""
echo "=== Casos de erro ==="

# sem argumentos
./scheduler > /dev/null 2>&1
roda_teste "sem argumentos → exit 1" "1" "$?"

# algoritmo inválido
./scheduler foo voo.txt > /dev/null 2>&1
roda_teste "algoritmo inválido → exit 1" "1" "$?"

# arquivo inexistente
./scheduler rate nao_existe.txt > /dev/null 2>&1
roda_teste "arquivo inexistente → exit 1" "1" "$?"

# gera arquivo malformado temporário
echo "abc" > /tmp/teste_malformado.txt
./scheduler rate /tmp/teste_malformado.txt > /dev/null 2>&1
roda_teste "tempoTotal não numérico → exit 1" "1" "$?"

echo "T1 10 5" > /tmp/teste_malformado2.txt
./scheduler rate /tmp/teste_malformado2.txt > /dev/null 2>&1
roda_teste "campo faltando (só 2 campos) → exit 1" "1" "$?"

echo "100" > /tmp/teste_malformado3.txt
echo "X 10 0 5" >> /tmp/teste_malformado3.txt
./scheduler rate /tmp/teste_malformado3.txt > /dev/null 2>&1
roda_teste "deadline = 0 (não positivo) → exit 1" "1" "$?"

echo "100" > /tmp/teste_malformado4.txt
echo "X 10 5 8" >> /tmp/teste_malformado4.txt
./scheduler rate /tmp/teste_malformado4.txt > /dev/null 2>&1
roda_teste "C > D (burst > deadline) → exit 1" "1" "$?"

echo "100" > /tmp/teste_malformado5.txt
echo "X 5 10 3" >> /tmp/teste_malformado5.txt
./scheduler rate /tmp/teste_malformado5.txt > /dev/null 2>&1
roda_teste "D > P (deadline > periodo) → exit 1" "1" "$?"

echo ""
echo "=== Resumo ==="
echo "Passou: $PASS / $TOTAL"
echo "Falhou: $FAIL / $TOTAL"

# cleanup
rm -f /tmp/teste_malformado*.txt

if [ $FAIL -eq 0 ]; then
    echo "Todos os testes passaram!"
    exit 0
else
    exit 1
fi