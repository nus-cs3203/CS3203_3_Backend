docker buildx build --platform linux/amd64 -t albertariel/cs3203_3_backend-pages_admin_analytics:0.0.7 -f ./services/pages/admin/analytics/Dockerfile .
docker push albertariel/cs3203_3_backend-pages_admin_analytics:0.0.7

docker buildx build --platform linux/amd64 -t albertariel/cs3203_3_backend-management:0.0.2 -f ./services/management/Dockerfile .
docker push albertariel/cs3203_3_backend-management:0.0.2