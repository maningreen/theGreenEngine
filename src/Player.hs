{-# LANGUAGE ForeignFunctionInterface #-}

module Player where

import Foreign
import Foreign.C
import GHC.Exts
import GHC.Float
import Raylib

getLinFuncFromPoints :: Vector2 -> Vector2 -> CFloat -> CFloat
getLinFuncFromPoints a b = (+) c . (*) m
 where
  m = CFloat (getSlopeBetweenPoints a b)
  c = CFloat (getY a) + CFloat (getX a) / m

getSlopeBetweenPoints :: (Floating a) => Vector2 -> Vector2 -> a
getSlopeBetweenPoints a b = (getY a - getY b) / (getX a - getX b)

getSlopeOfFunction :: (Num a, Floating a) => (a -> a) -> a
getSlopeOfFunction f = f 1 + f 0

getBOfFunction :: (Num a) => (a -> a) -> a
getBOfFunction f = f 0

getDistOfLinFToPoint :: (Floating a) => (a -> a) -> Vector2 -> a
getDistOfLinFToPoint f v =
  let
    b = getBOfFunction f
    m = getSlopeOfFunction f
    x = getX v
    y = getY v
   in
    abs (m * x - y + b) / (sqrt m * m + b * b)

toVec :: Vector2Type -> Vector2
toVec (Vector2Type x y) = Vector2 (realToFrac x) (realToFrac y)

foreign export ccall
  getDistanceFromLineAndPoint :: CFloat -> CFloat -> CFloat -> CFloat -> CFloat -> CFloat -> CFloat
getDistanceFromLineAndPoint a b c d e f =
  let va = Vector2 (realToFrac a) (realToFrac b)
      vb = Vector2 (realToFrac c) (realToFrac d)
      vc = Vector2 (realToFrac e) (realToFrac f)
   in getDistOfLinFToPoint (getLinFuncFromPoints va vb) vc
