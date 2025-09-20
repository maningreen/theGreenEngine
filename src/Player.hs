{-# LANGUAGE ForeignFunctionInterface #-}

module Player where

import Foreign.C
import Raylib

getLinFuncFromPoints :: (Floating a) => Vector2 -> Vector2 -> a -> a
getLinFuncFromPoints a b = (+ c) <$> (* m)
 where
  m = getSlopeBetweenPoints a b
  c = getY a

getSlopeBetweenPoints :: (Floating a) => Vector2 -> Vector2 -> a
getSlopeBetweenPoints a b = (getY a - getY b) / (getX a - getX b)

getSlopeOfFunction :: (Floating a) => (a -> a) -> a
getSlopeOfFunction f = f 1 - f 0

-- where f is a linear function
-- and x is the point on which you want to find the closest point on f to x
getClosestPointOnFunction :: (Float -> Float) -> Vector2 -> Vector2
getClosestPointOnFunction f (Vector2 x y)
  -- this means it's slope is infinity
  | 1 / m == 0 = Vector2 x $ m * x + y
  -- this means it's slope is 0
  | m == 0 = Vector2 x (realToFrac n)
  | otherwise = Vector2 commonClosest $ f commonClosest
 where
  n = f 0
  m = getSlopeOfFunction f
  m' = -recip m
  commonClosest = m' * (m' * x + n - y) / (m' * m' + 1)

foreign export ccall
  getDistanceFromLineAndPoint :: CFloat -> CFloat -> CFloat -> CFloat -> CFloat -> CFloat -> CFloat
getDistanceFromLineAndPoint :: (Floating a, Eq a, Real a) => a -> a -> a -> a -> a -> a -> a
getDistanceFromLineAndPoint a b c d e f =
  let va = Vector2 (realToFrac a) (realToFrac b)
      vb = Vector2 (realToFrac c) (realToFrac d)
      vc = Vector2 (realToFrac e) (realToFrac f)
      vClosest = getClosestPointOnFunction (getLinFuncFromPoints va vb) vc
   in dist vc vClosest
