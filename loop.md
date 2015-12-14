# Main Game Loop

while (!terminate)
{
	UpdateWorld();
	UpdatePlayer();
	MoveCamera();
	UpdateEntities();
	Draw();
}


## UpdateWorld

### Moving platforms
Update all moving platforms in the world every frame regardless of the camera super-region (the super-region is a bounding box larger than and entirely containing the camera view bounding box, the intention is that a subset of off-screen entities are updated).

## UpdatePlayer

When updating the player in UpdatePlayer everything else is considered static.

Things to think about:
*	Moving platforms
	When the player is standing on a moving platform the platform's movement needs to be added to the player move each frame
*	Entity collisions 

## MoveCamera

## UpdateEntities
Set of entities to update is based on the camera super-region.

Things to think about:
*	Player collisions
  