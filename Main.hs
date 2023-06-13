module Main where

import Parser

import Text.Megaparsec

import System.Environment
import Data.Text.IO

main :: IO ()
main = do
     args <- getArgs
     file <- Data.Text.IO.readFile (args !! 0)
     case parse ravenParser "" file of
          (Right a) -> print a
          (Left e) -> Prelude.putStr $ errorBundlePretty e
