# Build both images
docker build -f docker/compiler/Dockerfile -t deltanitt/codecharacter-compiler .
docker build -f docker/runner/Dockerfile -t deltanitt/codecharacter-runner .

# Push images to dockerhub
#docker push deltanitt/codecharacter-compiler
#docker push deltanitt/codecharacter-runner
