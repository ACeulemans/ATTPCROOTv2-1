#ifndef AtFITTER_H
#define AtFITTER_H

#include <Rtypes.h>
#include <TObject.h>

#include <tuple>
#include <vector>

class AtDigiPar;
class AtTrack;
class FairLogger;
class TBuffer;
class TClass;
class TMemberInspector;

namespace genfit {
class Track;
} // namespace genfit

namespace AtFITTER {

class AtFitter : public TObject {

public:
   AtFitter();
   virtual ~AtFitter();
   // virtual std::vector<AtTrack> GetFittedTrack() = 0;
   virtual genfit::Track *FitTracks(AtTrack *track) = 0;
   virtual void Init() = 0;

   void MergeTracks(std::vector<AtTrack> *trackCandSource, std::vector<AtTrack> *trackJunkSource,
                    std::vector<AtTrack> *trackDest, bool fitDirection, bool simulationConv);

protected:
   FairLogger *fLogger{}; ///< logger pointer
   AtDigiPar *fPar{};     ///< parameter container
   std::tuple<Double_t, Double_t>
   GetMomFromBrho(Double_t A, Double_t Z,
                  Double_t brho); ///< Returns momentum (in GeV) from Brho assuming M (amu) and Z;

   ClassDef(AtFitter, 1);
};

} // namespace AtFITTER

#endif
