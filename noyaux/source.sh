if [ "$1" == "2020" ]; then
  module load StdEnv/2020
  module load python/3.10 scipy-stack/2023a
else
  module load StdEnv/2023
  module load python/3.11 scipy-stack/2023b
fi
