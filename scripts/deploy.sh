docker buildx build --platform linux/amd64 -t albertariel/cs3203_3_backend-analytics:0.0.8 -f ./services/analytics/Dockerfile .
docker push albertariel/cs3203_3_backend-analytics:0.0.8

docker buildx build --platform linux/amd64 -t albertariel/cs3203_3_backend-management:0.0.3 -f ./services/management/Dockerfile .
docker push albertariel/cs3203_3_backend-management:0.0.3

docker buildx build --platform linux/amd64 -t albertariel/cs3203_3_backend-user:0.0.1 -f ./services/user/Dockerfile .
docker push albertariel/cs3203_3_backend-user:0.0.1