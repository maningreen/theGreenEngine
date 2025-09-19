module Raylib (Vector2(Vector2), HasXY(getX, getY), Vector2Type(Vector2Type), dist) where

import Foreign
import Foreign.C.Types

data Vector2Type = Vector2Type CFloat CFloat 
  deriving (Show, Eq) 
type CVector2 = Ptr Vector2Type 
data Vector2 = Vector2 Float Float 

class HasXY a where 
  getX :: Floating b => a -> b 
  getY :: Floating b => a -> b 
  -- just defining basic things for the Vector2 class 

dist :: Floating a => Vector2 -> Vector2 -> a
dist (Vector2 x1 y1) (Vector2 x2 y2) = sqrt $ (realToFrac x1 - realToFrac x2) ** 2 + (realToFrac y1 - realToFrac y2) ** 2

instance Storable Vector2Type where 
  sizeOf _ = 8 
  alignment = sizeOf 
  peek ptr = do 
    x <- peekByteOff ptr 0 
    y <- peekByteOff ptr 4 
    return (Vector2Type x y) 
  poke a = pokeElemOff a 0 

instance HasXY Vector2Type where 
  getX (Vector2Type x _) = realToFrac x 
  getY (Vector2Type _ y) = realToFrac y

instance HasXY Vector2 where 
  getX (Vector2 x _) = realToFrac x 
  getY (Vector2 _ y) = realToFrac y
