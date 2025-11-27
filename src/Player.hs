{-# LANGUAGE ForeignFunctionInterface #-}

module Player where

import Raylib
import Foreign (Storable(peek, poke))
import GHC.Exts (Ptr)

getLinFuncFromPoints :: (Floating a) => Vector2 -> Vector2 -> a -> a
getLinFuncFromPoints a b = (+ c) . (* m)
-- mx + b... except b is already a variable so it's c instead
 where
  m = getSlopeBetweenPoints a b
  c = -m * getX a + getY a

getSlopeBetweenPoints :: (Floating a) => Vector2 -> Vector2 -> a
getSlopeBetweenPoints (Vector2 x1 y1) (Vector2 x2 y2) = realToFrac $ (y2 - y1) / (x2 - x1)

getSlopeOfFunction :: (Num a) => (a -> a) -> a
getSlopeOfFunction f = f 1 - f 0

-- where f is a linear function
-- and x is the point on which you want to find the closest point on f to x
getClosestPointOnFunction :: (Float -> Float) -> Vector2 -> Vector2
getClosestPointOnFunction f (Vector2 x y)
  -- this means its slope is infinity
  -- and is kinda hard to do...
  -- | 1 / m <= 0.1 = Vector2 (f 0) y
  -- this means its slope is 0
  -- | m == 0 = Vector2 x $ f 0
  -- | otherwise
    = Vector2 commonClosest $ f commonClosest
  where
    m = getSlopeOfFunction f
    commonClosest = (x + (m * (y - f 0))) / (m ** 2 + 1)

getClosestPointOnFunctionBounded :: (Float -> Float) -> Float -> Float -> Vector2 -> Vector2
getClosestPointOnFunctionBounded func l r p@(Vector2 a _) = Vector2 clampedX $ func clampedX
  where
    (Vector2 x _) = getClosestPointOnFunction func p
    clampedX = clamp x
    clamp a = max minimum $ min a maximum
    minimum = min l r
    maximum = max l r

getClosestPointFromLineAndPoint :: Ptr CVector2 -> Ptr CVector2 -> Ptr CVector2 -> Ptr CVector2 -> IO ()
getClosestPointFromLineAndPoint a b c o = do
  aRl <- peek a
  bRl <- peek b
  cRl <- peek c
  let 
    aRat = cVecToVec aRl
    bRat = cVecToVec bRl
    cRat = cVecToVec cRl
    f = getLinFuncFromPoints aRat bRat
    pos = getClosestPointOnFunctionBounded f (getX aRat) (getX bRat) cRat
  poke o $ vecToCVec pos

foreign export ccall
  getClosestPointFromLineAndPoint :: Ptr CVector2 -> Ptr CVector2 -> Ptr CVector2 -> Ptr CVector2 -> IO ()
