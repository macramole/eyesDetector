class CurrentRectangle {
	public:
		ofRectangle rect;
		const int MIN_FRAMES = 4;
		const int AGE_OF_CONSENT = 5; //sera candidate hasta que cumpla AGE_OF_CONSENT
		const int MIN_INTERSECTION_AREA = 100; //mas chico hará que cuadros mas alejados sean seleccionados
		const int MIN_PROMEDIADOS_START  = 5; //luego de haber promediado por lo menos 5 cuadrados empezará a ajustarse

		int frames = 0;
		int age = 0; //sube uno cada MIN_FRAMES que pasaron
		bool hadRectanglesInside = false;
		bool isCandidate = true;
		ofPoint centroPromedio;
		unsigned int cantPromediados = 0;


		bool isEmpty() {
			return ( rect.width == 0 );
		}
		void setRectangle(ofRectangle rectangle) {
			rect = rectangle;
		}
		void reset() {
			rect.set(0,0,0,0);
			frames = 0;
			hadRectanglesInside = false;
			age = 1;
			isCandidate = true;
			centroPromedio.set(0,0,0);
		}
		void reset(ofRectangle newRectangle) {
			reset();
			rect = newRectangle;
			rect.scaleFromCenter(1.8);
		}
		void process( std::vector<ofxCvBlob> blobs ) {
			//si no hay nada que agarre el primero
		    if ( isEmpty() ) {
		        if ( !blobs.empty() ) {
					reset( getBiggestRectangle(blobs) );
		        }
		    } else {
				processCentroPromedio(blobs);

		        if ( !hadRectanglesInside ) {
		            if ( frames > MIN_FRAMES ) {
		                reset();
		            } else {
		                hadRectanglesInside = isAnyRectangleInside(blobs);
		            }
		        } else {
		            if ( frames > MIN_FRAMES * pow(1.4,age) ) { //la cantidad de frames requeridos va aumentando
		                frames = -1;
		                hadRectanglesInside = false;
						age++;
						if ( age >= AGE_OF_CONSENT ) {
							isCandidate = false;
						}
		            }
		        }
		        frames++;
		    }
		}
		bool isAnyRectangleInside( std::vector<ofxCvBlob> blobs ) {
		    if ( !isEmpty() ) {
		        for(int i = 0; i < blobs.size(); i++) {
		            ofRectangle intersection = rect.getIntersection( blobs[i].boundingRect );
		            if ( intersection.getArea() > MIN_INTERSECTION_AREA ) {
		                return true;
		            }
		        }
		    }
		    return false;
		}
		ofRectangle getBiggestRectangle( std::vector<ofxCvBlob> blobs ) {
			ofRectangle * biggestRectangle = new ofRectangle();
			for(int i = 0; i < blobs.size(); i++) {
				if ( blobs[i].boundingRect.getArea() > biggestRectangle->getArea() ) {
					biggestRectangle = &blobs[i].boundingRect;
				}
			}
			return *biggestRectangle;
		}
		void processCentroPromedio( std::vector<ofxCvBlob> blobs ) {
			if ( !isEmpty() ) {
				for(int i = 0; i < blobs.size(); i++) {
					ofRectangle intersection = rect.getIntersection( blobs[i].boundingRect );
		            if ( intersection.getArea() > MIN_INTERSECTION_AREA ) {
						centroPromedio += blobs[i].boundingRect.getCenter();
						centroPromedio = centroPromedio / 2;
						cantPromediados++;
		            }
				}

				if ( cantPromediados >= MIN_PROMEDIADOS_START ) {
					ofPoint centerError = rect.getCenter().interpolate( centroPromedio, 0.1 );
					rect.setFromCenter( centerError, rect.getWidth(), rect.getHeight() );
				}
			}
		}
};
