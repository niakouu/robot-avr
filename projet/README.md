# Projet 1

## Build

Le projet est divisé en `lib/` et `app/`. Chaque dossier possède son propre `Makefile`.  
Pour construire le projet, vous pouvez exécuter les commandes suivantes :

```sh
cd app
make
make install # si vous voulez l'installer sur un robot en fonctionnement
make clean # clean
```

Built files will be in `lib/build` or `app/build`.

## Structure

Le code fonctionne selon un modèle de boucle d'événements, où les événements sont envoyés à chaque gestionnaire de défi, soit `ForkChallengeHandler`, `HouseChallengeHandler`, soit `MazeChallengeHandler`.
