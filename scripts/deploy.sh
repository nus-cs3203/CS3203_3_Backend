docker buildx build --platform linux/amd64 -t albertariel/cs3203_3_backend-pages_admin_analytics:0.0.6 -f ./services/pages/admin/analytics/Dockerfile .
docker push albertariel/cs3203_3_backend-pages_admin_analytics:0.0.6