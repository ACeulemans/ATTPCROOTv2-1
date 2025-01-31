#ifndef ATPATTERNLINE_H
#define ATPATTERNLINE_H

#include "AtHit.h" // for XYZVector
#include "AtPattern.h"

#include <Math/Point3D.h>
#include <Math/Point3Dfwd.h> // for XYZPoint
#include <Rtypes.h>          // for THashConsistencyHolder, ClassDefOverride

#include <vector> // for vector

class TBuffer;
class TClass;
class TMemberInspector;

using XYZPoint = ROOT::Math::XYZPoint;

namespace AtPatterns {

/**
 * @brief Describes a linear track
 *
 * @ingroup AtPattern
 */
class AtPatternLine : public AtPattern {
public:
   AtPatternLine();

   XYZPoint GetPoint() const { return {fPatternPar[0], fPatternPar[1], fPatternPar[2]}; }
   XYZVector GetDirection() const { return {fPatternPar[3], fPatternPar[4], fPatternPar[5]}; }

   virtual void DefinePattern(const std::vector<XYZPoint> &points) override;
   virtual Double_t DistanceToPattern(const XYZPoint &point) const override;
   virtual XYZPoint ClosestPointOnPattern(const XYZPoint &point) const override;
   virtual XYZPoint GetPointAt(double z) const override;
   virtual TEveLine *GetEveLine() const override;
   virtual std::unique_ptr<AtPattern> Clone() const override { return std::make_unique<AtPatternLine>(*this); }

protected:
   virtual void FitPattern(const std::vector<XYZPoint> &points, const std::vector<double> &charge) override;
   double parameterAtPoint(const XYZPoint &point) const;
   ClassDefOverride(AtPatternLine, 1)
};
} // namespace AtPatterns
#endif //#ifndef ATPATTERNLINE_H
