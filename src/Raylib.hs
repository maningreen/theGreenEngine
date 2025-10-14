module Raylib (cVecToVec, vecToCVec, CVector2(..), Vector2(Vector2), HasXY(getX, getY), dist) where

import Foreign
import Foreign.C.Types

data CVector2 = CVector2 CFloat CFloat
  deriving (Show, Eq, Read)
data Vector2 = Vector2 Float Float 
  deriving (Show, Eq, Read)

class HasXY a where 
  getX :: Floating b => a -> b 
  getY :: Floating b => a -> b 
  -- just defining basic things for the Vector2 class 

dist :: Floating a => Vector2 -> Vector2 -> a
dist (Vector2 x1 y1) (Vector2 x2 y2) = sqrt $ ((realToFrac x1 - realToFrac x2) ** 2) + ((realToFrac y1 - realToFrac y2) ** 2)

cVecToVec :: CVector2 -> Vector2
cVecToVec (CVector2 a b) = Vector2 (realToFrac a) (realToFrac b)

vecToCVec :: Vector2 -> CVector2
vecToCVec (Vector2 a b) = CVector2 (realToFrac a) (realToFrac b)

instance Storable CVector2 where 
  sizeOf = const 8 
  alignment = const 4 
  peek ptr = do 
    x <- peekByteOff ptr 0 
    y <- peekByteOff ptr 4 
    return (CVector2 x y) 
  poke p (CVector2 a b) = pokeByteOff p 0 a >> pokeByteOff p 4 b

instance HasXY Vector2 where 
  getX (Vector2 x _) = realToFrac x 
  getY (Vector2 _ y) = realToFrac y

instance HasXY CVector2 where 
  getX (CVector2 x _) = realToFrac x 
  getY (CVector2 _ y) = realToFrac y
