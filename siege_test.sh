#!/bin/bash

HOST="http://127.0.0.1:8080"
CONCURRENCY=50
DURATION=30s
LOG=siege_results.log

echo "=== Siege Concurrency Stress Test: GET / ==="
echo "Target  : $HOST/"
echo "Users   : $CONCURRENCY concurrent"
echo "Duration: $DURATION"
echo ""

# baseline: single user
#echo "[1/3] Baseline (1 user)..."
#siege -c 1 -t 10s "$HOST/" 2>&1 | tee -a $LOG

#echo ""
#echo "[2/3] Medium load (20 users)..."
#siege -c 20 -t ${DURATION} "$HOST/" 2>&1 | tee -a $LOG

#echo ""
#echo "[3/3] Full stress (${CONCURRENCY} users)..."
siege -c $CONCURRENCY -t ${DURATION} "$HOST/" 2>&1 | tee -a $LOG

echo "=== Siege Concurrency Stress Test: GET / ==="
echo "Target  : $HOST/"
echo "Users   : $CONCURRENCY concurrent"
echo "Duration: $DURATION"
echo ""



echo ""
echo "=== Done. Results saved to $LOG ==="
