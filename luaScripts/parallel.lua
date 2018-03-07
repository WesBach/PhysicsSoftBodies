-- bool isInGroup = false;
-- bool isParallel = false;
-- float xPoint = 0.0f;
-- float yPoint = 0.0f;
-- float zPoint = 0.0f;
-- int ObjectIndex;
-- float time = 0.0f;


a = getNumObjectsParallel(4);

b = moveToDistanceTime(true,true, 120.0,100.0,0.0,0,10.0)

a = orientToTime(true,true,50.0,10.0,5.0,0,8.2)
--CurveYXZ(ObjectIndex,negPos,constVel,lengthOfCurve)
a = curveYXZ(true,true, 1, true,10.0,800)
--createCurveYZX(ObjectIndex, negPos)
a = curveYZX(true,true, 1, false)